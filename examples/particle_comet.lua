--[[
    Simple particle system
    This one uses the mouse location as the origin of the system.
    So, as you move the mouse around, the continuous stream of 
    particles will begin from wherever the mouse is currently located.

    Makes for a nice 'streamer' effect.

]]

local ffi = require("ffi")

local stats = require("P5Status")()


-- A simple Particle class
local Particle = {}
setmetatable(Particle, {
    __call = function(self, ...)
      return self:new(...)
    end;
})
local Particle_mt = {
  __index = Particle;
}

function Particle.new(self, position)
  local obj = {
    acceleration = PVector(0, 0.08);
    velocity = PVector(random(-1.0, 1.0), random(-2.0, 0));
    position = PVector(position);
    lifespan = 255;
  }
  setmetatable(obj, Particle_mt)

  return obj;
end

function Particle.run(self)
  self:update();
  self:display();
end

-- Method to update position
function Particle.update(self)
  self.velocity:add(self.acceleration);
  self.position:add(self.velocity);
  self.lifespan = self.lifespan - 2;
end

-- Method to display
function Particle.display(self)
  if not self:isDead() then
    stroke(200, self.lifespan);
    --noStroke()
    strokeWeight(2);
    fill(127, self.lifespan);
    ellipse(self.position.x, self.position.y, 12, 12);
  end
end

-- Is the particle still useful?
function Particle.isDead(self)
  return self.lifespan < 0;
end

-- Particle System
local ParticleSystem = {}
setmetatable(ParticleSystem, {
  __call = function(self, ...)
    return self:new(...)
  end;
})
local ParticleSystem_mt = {
  __index = ParticleSystem;
}

function ParticleSystem.new(self, position)
  local obj = {
    origin = position;
    particles = {};
  }
  setmetatable(obj, ParticleSystem_mt)

  return obj;
end

function ParticleSystem.addParticle(self)
  table.insert(self.particles, Particle(self.origin));
end

function ParticleSystem.run(self)
  for i = #self.particles,  1, -1 do
    local p = self.particles[i];
    p:run();
    if (p:isDead()) then
      table.remove(self.particles,1);
    end
  end
end



local systems = {}
local currentsystem;

local function addSystem(x, y)
    local system = ParticleSystem(PVector(x, y))
    table.insert(systems, system)
    return system
end

local function reset()
    systems = {}
    local x = width/2;
    local y = 50;

    if mouseX then
        x = mouseX;
        y = mouseY;
    end

    currentsystem = addSystem(x, y)    
end

function setup()
    reset()
end
  
function draw()
  --print("draw: ", #system.particles)
    background(51);
    for _, system in ipairs(systems) do
        system:addParticle();
        system:run();
    end
    
    stats:draw()
end


function mouseMoved()
    if currentsystem then
        currentsystem.origin = PVector(mouseX, mouseY)
    end
end

function mousePressed()
    currentsystem = addSystem(mouseX, mouseY)
end

function keyPressed()
    reset();
end

go {width =1280, height=1024, frameRate = 30, title="Particle Comet"}