local d3gl = require("d3gl")
local Colors = d3gl.Colors
local random_range = d3gl.mathx.random_range

local BACKGROUND = d3gl.hex("#0b1020")

local app = {}

local test = {}

function app.load()
    for i = 1, 10 do
        local x = random_range(0, d3gl.width)
        local y = random_range(0, d3gl.height)
        local r = 20
        local c = d3gl.hsv(random_range(0, 360), 1, 1)
        test[#test + 1] = { x = x, y = y, r = r, c = c}
    end
end

function app.update(dt)
end

function app.draw()
    d3gl.clear(BACKGROUND)
    d3gl.draw_text(8, 8, "AVIS", Colors.WHITE, 2)
    d3gl.draw_fps(8, d3gl.height - 12)
    print("test")
end

d3gl.run(app, { title = "avis" })
