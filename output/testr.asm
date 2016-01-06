ancillary "comment", 1, 4, "scripts/test.exl", ""
newclass .nil, "test.Point", .1
local_create .1, d3i1
ancillary "comment", 2, 7, "scripts/test.exl", ""
ancillary "comment", 3, 7, "scripts/test.exl", ""
ancillary "comment", 5, 4, "scripts/test.exl", ""
local_create .nil, d3i2
ancillary "comment", 6, 4, "scripts/test.exl", ""
local_create .nil, d3i3
function [d4i1, d4i2, d4i3], {
        local_create .nil, d4i4
        ancillary "comment", 7, 7, "scripts/test.exl", ""
        move d4i1, .1
        call_method .1, new, [], [.2]
        move .2, d4i4
        ancillary "comment", 8, 7, "scripts/test.exl", ""
        move d4i2, .3
        move d4i4, .4
        move .3, .4.d2i1
        ancillary "comment", 9, 7, "scripts/test.exl", ""
        move d4i3, .5
        move d4i4, .6
        move .5, .6.d2i2
        ancillary "comment", 10, 4, "scripts/test.exl", ""
        return [d4i4]
}, .2
move .2, d3i3
ancillary "comment", 11, 4, "scripts/test.exl", ""
move d3i3, .3
move d3i1, .4
move 10, .5
move 20, .6
call .3, [.4, .5, .6], [.7]
move .7, d3i2
ancillary "comment", 12, 4, "scripts/test.exl", ""
local_create .nil, d3i4
function [d4i1, d4i2], {
        ancillary "comment", 13, 7, "scripts/test.exl", ""
        move d3i3, .1
        move d3i1, .2
        move d4i1, .3
        move .3.d2i1, .4
        move d4i2, .5
        mul .4, .5, .6
        move d4i1, .7
        move .7.d2i2, .8
        move d4i2, .9
        mul .8, .9, .10
        call .1, [.2, .6, .10], [.11]
        move .11, d4i1
        ancillary "comment", 14, 4, "scripts/test.exl", ""
        return [d4i1]
}, .8
move .8, d3i4
ancillary "comment", 15, 4, "scripts/test.exl", ""
move d3i4, .9
move d3i2, .10
move 2, .11
call .9, [.10, .11], [.12]
move .12, d3i2
