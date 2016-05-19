local modname = ...
local choice = require('peg.choice')
local rule = require('peg.rule')
local utility
local prioritychoice = choice:module(modname)

prioritychoice.name = '(prioritychoice)'

function prioritychoice:init(it)
	self.rules = {}
	self.priorities = {}
	self.order = 0
	self.basecomparer = it.comparer or utility.operator.lt
	self.rulecomparer = function(lhs, rhs)
		local lp = self.priorities[lhs]
		local rp = self.priorities[rhs]
		if self.basecomparer(lp.base, rp.base) then
			return true
		elseif self.basecomparer(rp.base, lp.base) then
			return false
		else
			return lp.order < rp.order
		end
	end
	rule.init(self, it)
end

function prioritychoice:insert(rule, priority)
	self.order = self.order + 1
	self.priorities[rule] = {
		base = priority or 0,
		order = self.order}
	self.rules[self.order] = rule
	table.sort(self.rules, self.rulecomparer)
end

utility = require('base.utility')
