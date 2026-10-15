local Simulation = {}
Simulation.__index = Simulation

local function GetMaxValues(tables)
    local id = 0
    for _, v in pairs(tables) do
        if v.id > id then id = v.id end
    end
    return id
end


Simulation.VALUE = {
    EMPTY = {id = 0,colors = {r = 0, g = 0, b = 0}, cross = {}},
    WALL  = {id = 1, colors = {r = 0.5, g = 0.5, b = 0.5}, cross = {}},
    SAND  = {id = 2, colors = {r = 1, g = 0.85, b = 0.4}, cross = {}},
    WATER = {id = 3, colors = {r = 0.2, g = 0.5, b = 1.0}, cross = {}},
    --FIRE  = {id = 4, colors = {r = 1.0, g = 0.3, b = 0.1}, cross = {}},
    --WOOD  = {id = 5, colors = {r = 0.6, g = 0.4, b = 0.2}, cross = {}},
    --SMOKE = {id = 6, colors = {r = 0.8, g = 0.8, b = 0.8}, cross = {}},
    --STEAM = {id = 7, colors = {r = 0.9, g = 0.95, b = 1.0}, cross = {}},
    --ACID  = {id = 8, colors = {r = 0.3, g = 0.9, b = 0.1}, cross = {}},
    --STONE = {id = 9, colors = {r = 0.35, g = 0.35, b = 0.4}, cross = {}},
    --LAVA  = {id = 10, colors = {r = 1.0, g = 0.6, b = 0.0}, cross = {}},
}

Simulation.EMPTY_CELL = {color = {r = 0, g = 0, b = 0}, type = Simulation.VALUE.EMPTY.id}

local function getIndex(width, x, y)
    return (y * width) + x + 1
end

Simulation.MAX_VALUE = GetMaxValues(Simulation.VALUE)


function Simulation.add_cross_relation(v1, v2)
    local key1 = string.upper(v1)
    local key2 = string.upper(v2)
    if not Simulation.VALUE[key1] or not Simulation.VALUE[key2] then return end
    if not Simulation.VALUE[key1].cross then Simulation.VALUE[key1].cross = {} end
    if not Simulation.VALUE[key2].cross then Simulation.VALUE[key2].cross = {} end
    Simulation.VALUE[key1].cross[key2] = true
    Simulation.VALUE[key2].cross[key1] = true
end

function Simulation.GetTypeFromId(id)
    for k, v in pairs(Simulation.VALUE) do
        if v.id == id then
            return k
        end
    end
    return "EMPTY"
end

local function updateSand(sim, idx, x, y)
    local width = sim.width
    local below = getIndex(width, x, y + 1)

    if sim.grid[below].type == Simulation.VALUE.EMPTY.id then
        sim.grid[below] = sim.grid[idx]
        sim.grid[idx] = Simulation.EMPTY_CELL
        return
    end

    if Simulation.VALUE[Simulation.GetTypeFromId(sim.grid[below].type)].cross["SAND"] then
        local tmp = sim.grid[below]
        sim.grid[below] = sim.grid[idx]
        sim.grid[idx] = tmp
    end

    local dir = math.random() < 0.5 and -1 or 1
    local sideA = getIndex(width, x + dir, y + 1)
    local sideB = getIndex(width, x - dir, y + 1)
    
    local canGoA = (x + dir >= 0 and x + dir < width)
    local canGoB = (x - dir >= 0 and x - dir < width)

    if canGoA and sim.grid[sideA].type == Simulation.VALUE.EMPTY.id then
        sim.grid[sideA] = sim.grid[idx]
        sim.grid[idx] = Simulation.EMPTY_CELL
    elseif canGoB and sim.grid[sideB].type == Simulation.VALUE.EMPTY.id then
        sim.grid[sideB] = sim.grid[idx]
        sim.grid[idx] = Simulation.EMPTY_CELL
    end
end

local function updateWater(sim, idx, x, y)
    local width = sim.width
    local below = getIndex(width, x, y + 1)
    if sim.grid[below].type == Simulation.VALUE.EMPTY.id then
        sim.grid[below] = sim.grid[idx]
        sim.grid[idx] = Simulation.EMPTY_CELL
        return
    end

    local dir = math.random() < 0.5 and -1 or 1
    local sideDownA = getIndex(width, x + dir, y + 1)
    local sideDownB = getIndex(width, x - dir, y + 1)
    
    local canGoA = (x + dir >= 0 and x + dir < width)
    local canGoB = (x - dir >= 0 and x - dir < width)

    if canGoA and sim.grid[sideDownA].type == Simulation.VALUE.EMPTY.id then
        sim.grid[sideDownA] = sim.grid[idx]
        sim.grid[idx] = Simulation.EMPTY_CELL
        return
    elseif canGoB and sim.grid[sideDownB].type == Simulation.VALUE.EMPTY.id then
        sim.grid[sideDownB] = sim.grid[idx]
        sim.grid[idx] = Simulation.EMPTY_CELL
        return
    end

    local sideA = getIndex(width, x + dir, y) 
    local sideB = getIndex(width, x + dir, y)
    
    if canGoA and sim.grid[sideA].type == Simulation.VALUE.EMPTY.id then
        sim.grid[sideA] = sim.grid[idx]
        sim.grid[idx] = Simulation.EMPTY_CELL
    elseif canGoB and sim.grid[sideB].type == Simulation.VALUE.EMPTY.id then
        sim.grid[sideB] = sim.grid[idx]
        sim.grid[idx] = Simulation.EMPTY_CELL
    end
end


local function updateSmoke(sim, idx, x, y)
    local width = sim.width
    local above = getIndex(width, x, y - 1)
    if sim.grid[above] and sim.grid[above].type == Simulation.VALUE.EMPTY.id then
        sim.grid[above] = sim.grid[idx]
        sim.grid[idx] = Simulation.EMPTY_CELL
        return
    end

    if Simulation.VALUE[Simulation.GetTypeFromId(sim.grid[above].type)].cross["SMOKE"] then
        local tmp = sim.grid[above]
        sim.grid[above] = sim.grid[idx]
        sim.grid[idx] = tmp
    end

    local dir = math.random() < 0.5 and -1 or 1
    local sideUpA = getIndex(width, x + dir, y - 1)
    local sideUpB = getIndex(width, x - dir, y - 1)
    
    local canGoA = (x + dir >= 0 and x + dir < width) and sim.grid[sideUpA]
    local canGoB = (x - dir >= 0 and x - dir < width) and sim.grid[sideUpB]

    if canGoA and sim.grid[sideUpA].type == Simulation.VALUE.EMPTY.id then
        sim.grid[sideUpA] = sim.grid[idx]
        sim.grid[idx] = Simulation.EMPTY_CELL
        return
    elseif canGoB and sim.grid[sideUpB].type == Simulation.VALUE.EMPTY.id then
        sim.grid[sideUpB] = sim.grid[idx]
        sim.grid[idx] = Simulation.EMPTY_CELL
        return
    end

    local sideA = getIndex(width, x + dir, y) 
    local sideB = getIndex(width, x - dir, y)
    
    if canGoA and sim.grid[sideA].type == Simulation.VALUE.EMPTY.id then
        sim.grid[sideA] = sim.grid[idx]
        sim.grid[idx] = Simulation.EMPTY_CELL
    elseif canGoB and sim.grid[sideB].type == Simulation.VALUE.EMPTY.id then
        sim.grid[sideB] = sim.grid[idx]
        sim.grid[idx] = Simulation.EMPTY_CELL
    end
end

function Simulation.new(width, height)
    local self = setmetatable({}, Simulation)
    self.width = width
    self.height = height
    self.total_pixels = width * height
    self.grid = {}
    print(self.total_pixels)
    for i = 1, self.total_pixels do
        self.grid[i] = Simulation.EMPTY_CELL
    end
	print('a')
    return self
end

function Simulation:setCell(x, y, val, type)
    if x >= 0 and x < self.width and y >= 0 and y < self.height then
        local idx = getIndex(self.width, x, y)
        if self.grid[idx].type == Simulation.VALUE.EMPTY.id or self.grid[idx].type == Simulation.VALUE.WALL.id then
            local type_str = Simulation.GetTypeFromId(type)
            self.grid[idx] = {color = {
                r = Simulation.VALUE[type_str].colors.r * val, 
                g = Simulation.VALUE[type_str].colors.g * val,
                b = Simulation.VALUE[type_str].colors.b * val
            }, type = type}
        end
    end
end

function Simulation:update()
    for y = self.height - 2, 0, -1 do
        for x = 0, self.width - 1 do
            local idx = getIndex(self.width, x, y)
            local cell = self.grid[idx]
            if cell.type == Simulation.VALUE.SAND.id then
                updateSand(self, idx, x, y)
            elseif cell.type == Simulation.VALUE.WATER.id then
                updateWater(self, idx, x, y)
            elseif cell.type == Simulation.VALUE.SMOKE.id then
                updateSmoke(self, idx, x, y)
            end
            
        end
    end
end

return Simulation