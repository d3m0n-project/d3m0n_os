local d3gl = require("d3gl")
local Simulation = require("simulation")

local GAME_WIDTH = 320
local GAME_HEIGHT = 480 - 32
local SCALE = 3

local sim

d3gl.init(320, 480 - 32, function()
    sim = Simulation.new(GAME_WIDTH, GAME_HEIGHT)
    sim.add_cross_relation("sand", "water")
    sim.add_cross_relation("smoke", "water")
    rgbToHex({r = 10, g = 10, b = 10})
    print("tests")
    for i = 1, 5 do
        sim:setCell(20 + i, 20, 2)
        sim:setCell(20 + i, 23, 2)
        sim:setCell(20 + i, 24, 2)
    end
end)



function rgbToHex(rgb)
    local hexadecimal = '0X'

    for key, value in pairs(rgb) do
        local hex = ''

        while(value > 0)do
            local index = math.fmod(value, 16) + 1
            value = math.floor(value / 16)
            hex = string.sub('0123456789ABCDEF', index, index) .. hex
        end

        if(string.len(hex) == 0)then
            hex = '00'

        elseif(string.len(hex) == 1)then
            hex = '0' .. hex
        end

        hexadecimal = hexadecimal .. hex
    end
    print("hexadecimal", hexadecimal)
    return hexadecimal
end

function d3gl.draw()
    for x = 1, GAME_WIDTH do
        for y = 1, GAME_HEIGHT do
            local idx = (y * sim.width) + x + 1
            local val = sim.grid[idx].color

            d3gl.putpixel(x, y, rgbToHex(val))
        end
    end
    d3gl.update()
end