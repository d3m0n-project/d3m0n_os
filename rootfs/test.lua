
local d3gl = require("d3gl")
package.path = "/test_lua_avis/?.lua;" .. package.path

local config = require("simulation")
local bird = require("bird")

local random_range = d3gl.mathx.random_range
local vec2 = d3gl.vec2
local clear = d3gl.clear

local BACKGROUND = d3gl.hex("#0b1020")

local app = {}
local simulation = { boids = {} }

function app.load()
    local boids = simulation.boids

	clear(BACKGROUND)

    for i = 1, config.boids do
        local x = random_range(0, d3gl.width)
        local y = random_range(0, d3gl.height)

        boids[i] = bird(
            i,
            vec2(x, y),
            math.pi / 4,
            boids
        )
    end
end

function app.update(dt)
    local boids = simulation.boids
    local count = #boids

    for i = 1, count do
        boids[i]:snapshot()
    end

    for i = 1, count do
        boids[i]:update(dt)
    end
end

function app.draw()
    local boids = simulation.boids
    for i = 1, #boids do
        boids[i]:draw()
    end
end

d3gl.run(app, { title = "avis" })
