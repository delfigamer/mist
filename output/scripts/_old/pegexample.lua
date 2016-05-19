local peg = require('peg')
local syntaxscript = [[
	digit := [0-9];
	blank := *[\00-\20];

	integer {
		whole = $concat %digit,
	};

	fixed_w := fixed {
		whole = $concat %digit,
		'.',
		frac = $concat *digit,
	};
	fixed_f := fixed {
		whole = $concat *digit,
		'.',
		frac = $concat %digit,
	};
	fixed := fixed_w / fixed_f;

	float_w := float {
		whole = $concat %digit,
		?( '.' ),
		frac = $concat *digit,
		'e' / 'E',
		expsign = ?( '+' / '-' ),
		exp = $concat %digit,
	};
	float_f := float {
		whole = $concat *digit,
		'.',
		frac = $concat %digit,
		'e' / 'E',
		expsign = ?( '+' / '-' ),
		exp = $concat %digit,
	};
	float := float_w / float_f;

	$pr constname;
	constname <= 'pi';
	constname <= 'e';

	const {
		name = constname};

	number := const / float / fixed / integer;

	$function binop;
		$return $binary $narg 3 {$arg 1, blank < $arg 2 > blank, $assert $arg 1};
	$end;

	subexpression := '(' blank < $assert expression > blank $assert ')';

	number_or_subexpression := number / subexpression;

	power := $call binop {
		number_or_subexpression,
		'^',
		$name power,
	};

	$pr function;
	function <= 'sin';
	function <= 'cos';
	function <= 'tg';
	function <= 'ctg';
	function <= 'sh';
	function <= 'ch';
	function <= 'cth';
	function <= 'th';
	function <= 'ln';
	function <= 'lg';

	functioncall {
		function = function,
		blank,
		argument = $assert functioncall_or_power,
	};

	functioncall_or_power := functioncall / power;

	product := $call binop {
		functioncall_or_power,
		'*' / '/',
		$name product,
	};

	negation {
		'-' blank,
		arg = $assert product,
	};

	$binary sum {product / negation, blank < ('+' / '-') > blank, product,
	};

	expression := sum;

	$return blank < expression > blank $assert $eof;
]]
local parser = peg.loadstring(syntaxscript)

local op = {}

local function eval(el, ...)
	return assert(op[el.name])(el, ...)
end

function op.integer(el)
	return 'V_INT ' .. el._whole
end

function op.fixed(el)
	return 'V_FIXED ' .. el._whole .. '.' .. el._frac
end

function op.float(el)
	return 'V_FLOAT ' .. el._whole .. '.' .. el._frac .. 'E' .. (el._expsign or '') .. el._exp
end

local constants = {
	pi = 'C_PI',
	e = 'C_E',
}

function op.const(el)
	return assert(constants[el._name])
end

function op.power(el)
	return string.format('%s\n%s\nB_POWER', eval(el.prefix), eval(el.suffix))
end

local functions = {
	sin = 'F_SIN',
	cos = 'F_COS',
	tg = 'F_TAN',
	ctg = 'F_TAN\nF_INVERT',
	sh = 'F_SINH',
	ch = 'F_COSH',
	th = 'F_TANH',
	cth = 'F_TANH\nF_INVERT',
	ln = 'F_LOG',
	lg = 'F_LOG\nC_LN10\nB_DIVIDE',
}

function op.functioncall(el)
	return string.format('%s\n%s', eval(el._argument), assert(functions[el._function]))
end

local ops = {
	['*'] = 'B_MULTIPLY',
	['/'] = 'B_DIVIDE',
	['+'] = 'B_ADD',
	['-'] = 'B_SUBTRACT',
}

function op.product(el)
	return string.format('%s\n%s\n%s', eval(el.prefix), eval(el.suffix), assert(ops[el.infix]))
end

function op.negation(el)
	return string.format('%s\nF_NEGATE', eval(el._arg))
end

function op.sum(el)
	return string.format('%s\n%s\n%s', eval(el.prefix), eval(el.suffix), assert(ops[el.infix]))
end

local function main()
	local teststr = [[
		ln e^20 / 10 * 2 / 2 / 0.5^(-1)^2^.5 + 10 - 2 - 3 * (- cos pi)]]
	local tree = select(3, parser:invoke(peg.stream:create(teststr)))
	print(tree)
	print(eval(tree))
end

assert(pcall(main))
