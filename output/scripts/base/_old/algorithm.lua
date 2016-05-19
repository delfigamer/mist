local modname = ...
module(modname, package.seeall)

function adjustindex(index, length)
	if index < 0 then
		index = length + index + 1
		if index < 0 then
			return 1
		else
			return index
		end
	else
		if index > length then
			return length
		else
			return index
		end
	end
end

function adjustbounds(array, lb, rb)
	local arrlen = #array
	if rb and rb > arrlen then
		arrlen = rb
	end
	if lb then
		lb = adjustindex(lb, arrlen)
	else
		lb = 1
	end
	if rb then
		rb = adjustindex(rb, arrlen)
	else
		rb = arrlen
	end
	if lb > rb then
		rb = lb - 1
	end
	return lb, rb
end

function defaultcomparer(a, b)
	if a < b then
		return -1
	elseif b < a then
		return 1
	else
		return 0
	end
end

local function binsearch_r(array, target, lb, rb, comparer)
	if lb > rb then
		return false, lb
	end
	local mid = bit.rshift(lb + rb, 1)
	local cr = comparer(array[mid], target)
	if cr < 0 then
		return binsearch_r(array, target, mid + 1, rb, comparer)
	elseif cr > 0 then
		return binsearch_r(array, target, lb, mid - 1, comparer)
	else
		return true, mid
	end
end

function binsearch(array, target, lb, rb, comparer)
	lb, rb = adjustbounds(array, lb, rb)
	if not comparer then
		comparer = defaultcomparer
	end
	return binsearch_r(array, target, lb, rb, comparer)
end

function move(array, delta, lb, rb)
	lb, rb = adjustbounds(array, lb, rb)
	if delta > 0 then
		for i = rb, lb, -1 do
			array[i + delta] = array[i]
		end
	elseif delta < 0 then
		for i = lb, rb do
			array[i + delta] = array[i]
		end
	end
end
