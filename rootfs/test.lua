-- math_builtins_test.lua
-- Lua 5.1 - 5.4 Math Library Stress Test
-- Tests only the math library.
-- Run: lua math_builtins_test.lua

local M = math
local passed, failed, total = 0, 0, 0

local function test(name, fn)
    total = total + 1
    local ok, err = pcall(fn)

    if ok then
        passed = passed + 1
        print("[PASS] " .. name)
    else
        failed = failed + 1
        print("[FAIL] " .. name .. ": " .. tostring(err))
    end
end

local function eq(a, b)
    assert(a == b, tostring(a) .. " ~= " .. tostring(b))
end

local function near(a, b, eps)
    if a == b then return end -- also handles equal infinities
    eps = eps or 1e-6
    assert(
        math.abs(a - b) <= eps * math.max(1, math.abs(b)),
        tostring(a) .. " not near " .. tostring(b)
    )
end

local function is_nan(x)
    return x ~= x
end

local function is_inf(x)
    return x == math.huge or x == -math.huge
end

local function finite(x)
    return x == x and x ~= math.huge and x ~= -math.huge
end

print("========================================")
print(" Lua Math Library Built-in Test Suite")
print("========================================")

-- CONSTANTS

test("math.pi", function()
    assert(type(M.pi) == "number")
    near(M.pi, 3.141592653589793)
end)



test("math.huge", function()
    eq(M.huge, 1 / 0)
    assert(M.huge > 3e38)
end)


test("math.maxinteger (Lua 5.3+)", function()
    if M.maxinteger then
        assert(type(M.maxinteger) == "number")
        assert(M.maxinteger > 0)
    end
end)

test("math.mininteger (Lua 5.3+)", function()
    if M.mininteger then
        assert(type(M.mininteger) == "number")
        assert(M.mininteger < 0)
    end
end)

-- ABS

test("math.abs positive", function()
    eq(M.abs(10), 10)
end)

test("math.abs negative", function()
    eq(M.abs(-10), 10)
end)

test("math.abs zero", function()
    eq(M.abs(0), 0)
end)

test("math.abs float", function()
    near(M.abs(-3.75), 3.75)
end)

test("math.abs infinity", function()
    eq(M.abs(-M.huge), M.huge)
end)

test("math.abs NaN", function()
    assert(is_nan(M.abs(0 / 0)))
end)

-- ACOS

test("math.acos(1)", function()
    near(M.acos(1), 0)
end)

test("math.acos(0)", function()
    near(M.acos(0), M.pi / 2)
end)

test("math.acos(-1)", function()
    near(M.acos(-1), M.pi)
end)

test("math.acos midpoint", function()
    near(M.acos(0.5), M.pi / 3)
end)

test("math.acos out of domain", function()
    assert(is_nan(M.acos(2)))
end)

-- ASIN

test("math.asin(0)", function()
    near(M.asin(0), 0)
end)

test("math.asin(1)", function()
    near(M.asin(1), M.pi / 2)
end)

test("math.asin(-1)", function()
    near(M.asin(-1), -M.pi / 2)
end)

test("math.asin(0.5)", function()
    near(M.asin(0.5), M.pi / 6)
end)

test("math.asin out of domain", function()
    assert(is_nan(M.asin(-2)))
end)

-- ATAN / ATAN2

test("math.atan(0)", function()
    near(M.atan(0), 0)
end)

test("math.atan(1)", function()
    near(M.atan(1), M.pi / 4)
end)

test("math.atan(-1)", function()
    near(M.atan(-1), -M.pi / 4)
end)

test("math.atan large number", function()
    near(M.atan(1e100), M.pi / 2)
end)

test("math.atan2 if available", function()
    if M.atan2 then
        near(M.atan2(1, 1), M.pi / 4)
        near(M.atan2(1, -1), 3 * M.pi / 4)
        near(M.atan2(-1, -1), -3 * M.pi / 4)
        near(M.atan2(-1, 1), -M.pi / 4)
    end
end)

test("math.atan(y, x)", function()
    near(M.atan(1, 1), M.pi / 4)
    near(M.atan(1, -1), 3 * M.pi / 4)
end)

-- CEIL / FLOOR

test("math.ceil positive", function()
    eq(M.ceil(3.2), 4)
end)

test("math.ceil negative", function()
    eq(M.ceil(-3.2), -3)
end)

test("math.ceil integer", function()
    eq(M.ceil(5), 5)
end)

test("math.ceil zero", function()
    eq(M.ceil(0), 0)
end)

test("math.floor positive", function()
    eq(M.floor(3.9), 3)
end)

test("math.floor negative", function()
    eq(M.floor(-3.2), -4)
end)

test("math.floor integer", function()
    eq(M.floor(5), 5)
end)

test("math.floor zero", function()
    eq(M.floor(0), 0)
end)

test("math.floor infinity", function()
    eq(M.floor(M.huge), M.huge)
end)

-- COS / SIN / TAN

test("math.cos(0)", function()
    near(M.cos(0), 1)
end)

test("math.cos(pi)", function()
    near(M.cos(M.pi), -1)
end)

test("math.cos(pi/2)", function()
    near(M.cos(M.pi / 2), 0)
end)

test("math.cos periodicity", function()
    near(M.cos(2 * M.pi), 1)
end)

test("math.sin(0)", function()
    near(M.sin(0), 0)
end)

test("math.sin(pi/2)", function()
    near(M.sin(M.pi / 2), 1)
end)

test("math.sin(pi)", function()
    near(M.sin(M.pi), 0)
end)

test("math.sin(-pi/2)", function()
    near(M.sin(-M.pi / 2), -1)
end)

test("math.sin periodicity", function()
    near(M.sin(2 * M.pi), 0)
end)

test("math.tan(0)", function()
    near(M.tan(0), 0)
end)

test("math.tan(pi/4)", function()
    near(M.tan(M.pi / 4), 1)
end)

test("math.tan(-pi/4)", function()
    near(M.tan(-M.pi / 4), -1)
end)

-- DEG / RAD

test("math.deg(0)", function()
    near(M.deg(0), 0)
end)

test("math.deg(pi)", function()
    near(M.deg(M.pi), 180)
end)

test("math.deg(pi/2)", function()
    near(M.deg(M.pi / 2), 90)
end)

test("math.deg(2*pi)", function()
    near(M.deg(2 * M.pi), 360)
end)

test("math.rad(0)", function()
    near(M.rad(0), 0)
end)

test("math.rad(180)", function()
    near(M.rad(180), M.pi)
end)

test("math.rad(90)", function()
    near(M.rad(90), M.pi / 2)
end)

test("math.rad(360)", function()
    near(M.rad(360), 2 * M.pi)
end)

test("degree-radian round trip", function()
    for i = -720, 720, 15 do
        near(M.deg(M.rad(i)), i)
    end
end)

-- EXP / LOG

test("math.exp(0)", function()
    near(M.exp(0), 1)
end)

test("math.exp(1)", function()
    near(M.exp(1), M.exp(1))
    near(M.log(M.exp(1)), 1)
end)

test("math.exp(2)", function()
    near(M.exp(2), M.exp(1) ^ 2)
end)

test("math.exp(-1)", function()
    near(M.exp(-1), 1 / M.exp(1))
end)

test("math.log(1)", function()
    near(M.log(1), 0)
end)

test("math.log(e)", function()
    near(M.log(M.exp(1)), 1)
end)

test("math.log(10)", function()
    near(M.log(10), 2.302585092994046)
end)

test("math.log(0)", function()
    assert(M.log(0) == -M.huge or is_inf(M.log(0)))
end)

test("math.log negative", function()
    assert(is_nan(M.log(-1)))
end)

test("math.log base 10", function()
    near(M.log(100, 10), 2)
end)

test("math.log base 2", function()
    near(M.log(8, 2), 3)
end)

test("math.log base 3", function()
    near(M.log(81, 3), 4)
end)

test("math.log with base e", function()
    near(M.log(7, M.exp(1)), M.log(7))
end)

-- MODF

test("math.modf positive", function()
    local i, f = M.modf(3.75)
    eq(i, 3)
    near(f, 0.75)
end)

test("math.modf negative", function()
    local i, f = M.modf(-3.75)
    eq(i, -3)
    near(f, -0.75)
end)

test("math.modf integer", function()
    local i, f = M.modf(5)
    eq(i, 5)
    eq(f, 0)
end)

test("math.modf zero", function()
    local i, f = M.modf(0)
    eq(i, 0)
    eq(f, 0)
end)

test("math.modf infinity", function()
    local i, f = M.modf(M.huge)
    eq(i, M.huge)
    eq(f, 0)
end)

-- F MOD

test("math.fmod positive", function()
    eq(M.fmod(10, 3), 1)
end)

test("math.fmod negative dividend", function()
    eq(M.fmod(-10, 3), -1)
end)

test("math.fmod negative divisor", function()
    eq(M.fmod(10, -3), 1)
end)

test("math.fmod fractional", function()
    near(M.fmod(5.5, 2), 1.5)
end)

test("math.fmod exact division", function()
    eq(M.fmod(12, 3), 0)
end)

test("math.fmod division by zero", function()
    assert(not pcall(M.fmod, 1, 0))
end)

-- MIN / MAX

test("math.max two numbers", function()
    eq(M.max(1, 5), 5)
end)

test("math.max multiple numbers", function()
    eq(M.max(1, 5, 3, 9, 2), 9)
end)

test("math.max negative numbers", function()
    eq(M.max(-10, -2, -50), -2)
end)

test("math.max mixed values", function()
    eq(M.max(-100, 0, 100), 100)
end)

test("math.max one argument", function()
    eq(M.max(42), 42)
end)

test("math.min two numbers", function()
    eq(M.min(1, 5), 1)
end)

test("math.min multiple numbers", function()
    eq(M.min(1, 5, 3, -9, 2), -9)
end)

test("math.min negative numbers", function()
    eq(M.min(-10, -2, -50), -50)
end)

test("math.min mixed values", function()
    eq(M.min(-100, 0, 100), -100)
end)

test("math.min one argument", function()
    eq(M.min(42), 42)
end)

test("math.min/max infinities", function()
    eq(M.min(M.huge, 1), 1)
    eq(M.max(-M.huge, 1), 1)
end)

-- SQRT

test("math.sqrt zero", function()
    eq(M.sqrt(0), 0)
end)

test("math.sqrt one", function()
    eq(M.sqrt(1), 1)
end)

test("math.sqrt four", function()
    eq(M.sqrt(4), 2)
end)

test("math.sqrt nine", function()
    eq(M.sqrt(9), 3)
end)

test("math.sqrt fractional", function()
    near(M.sqrt(0.25), 0.5)
end)

test("math.sqrt large", function()
    near(M.sqrt(1e100), 1e50)
end)

test("math.sqrt negative", function()
    assert(is_nan(M.sqrt(-1)))
end)

test("math.sqrt infinity", function()
    eq(M.sqrt(M.huge), M.huge)
end)

-- POWERS

test("power operator positive", function()
    eq(2 ^ 3, 8)
end)

test("power operator zero", function()
    eq(5 ^ 0, 1)
end)

test("power operator negative exponent", function()
    near(2 ^ -3, 0.125)
end)

test("power operator fractional exponent", function()
    near(9 ^ 0.5, 3)
end)

test("power operator negative base integer exponent", function()
    eq((-2) ^ 4, 16)
    eq((-2) ^ 3, -8)
end)

test("power operator negative base fractional exponent", function()
    assert(is_nan((-1) ^ 0.5))
end)

test("power operator zero negative exponent", function()
    assert(not finite(0 ^ -1) or (0 ^ -1) == M.huge)
end)

-- FREXP / LDEXP

test("math.frexp if available", function()
    if M.frexp then
        local mantissa, exponent = M.frexp(8)
        near(mantissa, 0.5)
        eq(exponent, 4)
    end
end)

test("math.frexp fraction if available", function()
    if M.frexp then
        local mantissa, exponent = M.frexp(0.75)
        near(mantissa * 2 ^ exponent, 0.75)
    end
end)

test("math.ldexp if available", function()
    if M.ldexp then
        eq(M.ldexp(0.5, 4), 8)
        eq(M.ldexp(0.5, 1), 1)
        eq(M.ldexp(1, 0), 1)
    end
end)

-- TOINTEGER / TYPE (Lua 5.3+)

test("math.type integer", function()
    if M.type then
        eq(M.type(42), "integer")
    end
end)

test("math.type float", function()
    if M.type then
        eq(M.type(42.0), "float")
    end
end)

test("math.type non-number", function()
    if M.type then
        eq(M.type("42"), nil)
    end
end)

test("math.tointeger integer", function()
    if M.tointeger then
        eq(M.tointeger(42), 42)
    end
end)

test("math.tointeger integral float", function()
    if M.tointeger then
        eq(M.tointeger(42.0), 42)
    end
end)

test("math.tointeger fractional number", function()
    if M.tointeger then
        eq(M.tointeger(42.5), nil)
    end
end)

test("math.tointeger string", function()
    if M.tointeger then
        eq(M.tointeger("42"), 42)
    end
end)

test("math.tointeger NaN", function()
    if M.tointeger then
        eq(M.tointeger(0 / 0), nil)
    end
end)

-- FLOOR DIVISION (Lua 5.3+)

test("math.floor division operator", function()
    if _VERSION >= "Lua 5.3" then
        eq(7 // 2, 3)
        eq(-7 // 2, -4)
        eq(7 // -2, -4)
        eq(-7 // -2, 3)
    end
end)

-- ULT (Lua 5.3+)

test("math.ult if available", function()
    if M.ult then
        assert(M.ult(1, 2))
        assert(not M.ult(2, 1))
        assert(not M.ult(5, 5))
    end
end)

-- FMA (Lua 5.3+)

test("math.fma if available", function()
    if M.fma then
        near(M.fma(2, 3, 4), 10)
        near(M.fma(0, 10, 5), 5)
        near(M.fma(-2, 3, 4), -2)
    end
end)

-- ISNAN / ISINF / ISFINITE are helper checks here.
-- Lua does not provide these as standard math functions.

test("NaN detection", function()
    assert(is_nan(0 / 0))
    assert(not is_nan(1))
end)

test("infinity detection", function()
    assert(is_inf(M.huge))
    assert(is_inf(-M.huge))
    assert(not is_inf(0))
end)

test("finite number detection", function()
    assert(finite(1))
    assert(finite(-100))
    assert(not finite(M.huge))
    assert(not finite(0 / 0))
end)

-- RANDOM

test("math.random basic", function()
    local n = M.random()
    assert(n >= 0 and n < 1)
end)

test("math.random integer range", function()
    for i = 1, 100 do
        local n = M.random(1, 10)
        assert(n >= 1 and n <= 10)
        assert(n == M.floor(n))
    end
end)

test("math.random single bound", function()
    for i = 1, 100 do
        local n = M.random(5)
        assert(n >= 1 and n <= 5)
    end
end)

test("math.random negative range", function()
    for i = 1, 100 do
        local n = M.random(-10, -1)
        assert(n >= -10 and n <= -1)
    end
end)

test("math.random crossing zero", function()
    for i = 1, 100 do
        local n = M.random(-5, 5)
        assert(n >= -5 and n <= 5)
    end
end)

test("math.random invalid range", function()
    assert(not pcall(M.random, 10, 1))
end)

test("math.randomseed", function()
    local ok = pcall(M.randomseed, 12345)
    assert(ok)
end)

test("math.random repeatability", function()
    M.randomseed(12345)
    local a = M.random()

    M.randomseed(12345)
    local b = M.random()

    near(a, b)
end)

test("math.randomseed two arguments if supported", function()
    local ok = pcall(M.randomseed, 12345, 67890)
    assert(ok)
end)

-- RANDOM DISTRIBUTION SANITY CHECK
-- Statistical check only; not a proof of randomness.

test("math.random distribution sanity", function()
    M.randomseed(54321)

    local n = 10000
    local sum = 0

    for i = 1, n do
        sum = sum + M.random()
    end

    local average = sum / n
    assert(average > 0.45 and average < 0.55,
        "average outside expected broad range: " .. average)
end)

-- LARGE INPUTS

test("large absolute value", function()
    eq(M.abs(-1e300), 1e300)
end)

test("large square root", function()
    near(M.sqrt(1e300), 1e150)
end)

test("large exponential", function()
    assert(M.exp(80) > 1e30)
end)

test("exponential overflow behavior", function()
    local x = M.exp(10000)
    assert(is_inf(x) or x > 1e300)
end)

test("tiny exponential", function()
    local x = M.exp(-1000)
    assert(x >= 0)
end)

test("large trigonometric input", function()
    assert(finite(M.sin(1e30)))
    assert(finite(M.cos(1e30)))
end)

test("large logarithm", function()
    near(M.log(1e30), 30 * M.log(10))
end)

-- FLOATING POINT PRECISION

test("floating point addition", function()
    near(0.1 + 0.2, 0.3)
end)

test("floating point multiplication", function()
    near(0.1 * 10, 1)
end)

test("floating point division", function()
    near(1 / 10 * 10, 1)
end)

test("floating point cancellation", function()
    assert(finite((1e16 + 1) - 1e16))
end)

test("trigonometric identity", function()
    for i = -100, 100 do
        local x = i / 10
        near(M.sin(x)^2 + M.cos(x)^2, 1, 1e-6)
    end
end)

test("sqrt square identity", function()
    for i = 0, 1000 do
        local x = i / 10
        near(M.sqrt(x * x), x)
    end
end)

test("exp log identity", function()
    for i = -10, 10 do
        local x = M.exp(i / 2)
        near(M.log(x), i / 2, 1e-6)
    end
end)

-- FUNCTION PRESENCE

test("standard math functions exist", function()
    local names = {
        "abs", "acos", "asin", "atan",
        "ceil", "cos", "deg", "exp",
        "floor", "fmod", "log", "max",
        "min", "modf", "rad", "random",
        "randomseed", "sin", "sqrt", "tan",
    }

    for _, name in ipairs(names) do
        assert(type(M[name]) == "function",
            "missing math." .. name)
    end
end)

test("newer math functions when available", function()
    local names = {
        "ult", "tointeger", "type", "fma",
        "maxinteger", "mininteger",
    }

    for _, name in ipairs(names) do
        if M[name] ~= nil then
            if name ~= "maxinteger" and name ~= "mininteger" then
                assert(type(M[name]) == "function",
                    "math." .. name .. " should be a function")
            end
        end
    end
end)

-- RANDOM RANGE STRESS

test("random stress 1..100000", function()
    for i = 1, 1000 do
        local n = M.random(1, 100000)
        assert(n >= 1 and n <= 100000)
    end
end)

test("random single-value range", function()
    for i = 1, 20 do
        eq(M.random(7, 7), 7)
    end
end)

-- INTEGRAL ROUNDING STRESS

test("ceil/floor consistency", function()
    for i = -10000, 10000 do
        local x = i / 100
        assert(M.floor(x) <= x)
        assert(M.ceil(x) >= x)
        assert(M.ceil(x) - M.floor(x) <= 1)
    end
end)

test("floor idempotence", function()
    for i = -1000, 1000 do
        local x = i / 7
        eq(M.floor(M.floor(x)), M.floor(x))
    end
end)

test("ceil idempotence", function()
    for i = -1000, 1000 do
        local x = i / 7
        eq(M.ceil(M.ceil(x)), M.ceil(x))
    end
end)

-- ABSOLUTE VALUE STRESS

test("absolute value symmetry", function()
    for i = -10000, 10000 do
        local x = i / 13
        near(M.abs(x), M.abs(-x))
    end
end)

test("absolute value nonnegative", function()
    for i = -10000, 10000 do
        assert(M.abs(i / 17) >= 0)
    end
end)

-- MIN/MAX STRESS

test("min/max ordering", function()
    for i = -1000, 1000 do
        local a = i / 3
        local b = i / 7
        assert(M.min(a, b) <= M.max(a, b))
    end
end)