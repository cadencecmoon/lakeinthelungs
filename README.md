# Lake in the Lungs
I'm a CS student from Poland, just having fun with this gamedev-related project of mine.
My goal is to make a 3D raytracer in Vulkan, and if things work out, to fully flesh out an actual video game from this.
The *A Moonlit Walk Engine* is part of this project, and it's optimized (or limited I guess) to the stuff I need 
(not true, I *can't stop myself* from not implementing more than I need).

The engine is open, feel free to take my code, change stuff, use it in your own applications/video games.
I don't expect any contributions, but if you're a specialized magician with esoteric skills and you're interested enough, 
I would appreciate your input to optimizing or growing this project.
It's published under a MIT license. This is true for source code of the game, shaders etc. too.
There won't be art assets in this repository, and if I'll publish any they will be licensed in different terms.

I use C99 and assembly, for now targetting 64-bit platforms like x86\_64, ARM and RISC-V.
There are no real dependencies except the compiler - not even the standard C library, and the engine is supposed to run 
(in a limited way) without host support for stuff like audio or Vulkan drivers.
Vulkan is my rendering API of choice for this project. 
I'm not sure i'll implement other rendering backends, like DirectX12, Metal or WebGPU, that's not really important now.
But I still want to make sure that the main platforms I'm interested in will be supported, or that porting to a new platform will be straighforward.

Oh, and today Vulkan 1.4 was released. So I'll take that into account when implementing the renderer from scratch. 
Dynamic local read extension was promoted to core 1.4, very neat indeed.

## Some thoughts about platform support
The project is in very early development. For now it's an amalgamation of different stuff I've done in the past, trying to connect them in a way that makes sense, 
expand where they were lacking and trying to figure out what the hell is going on, actually.
But the goal is to have fun, and I sure do. Maybe it will be a base for some educational content in the future, or maybe content for some masturbation on r/gamedev.

Only the x86\_64 Linux platform is *really* supported right now, cause that is my main environment I work in.
I plan to port the engine to ARM and RISC-V, so I can test it on some SBC's, mobile platforms and some potato PCs.
A potato benchmark sounds fun, and it will probably crush my dreams of optimizing this game at all.

For architectures other than x86, ARM and RISC-V, i'm not sure when or if ever i'll find the time to port this, 
and I have no reason for this if I don't have the hardware to test this stuff - 
it's the case too if no one else with such exotic hardware actually needs this port as well.

I promise I will dual boot Windows on my laptop *eventually*, so I can actually port this stuff to Windows.
And for MacOS, I must say, I don't own an Apple device at all. 
So any effort I make for implementing the MACH-O ABI assembly, Cocoa, CoreAudio or some another OBJ-C code specific API, it's futile. 
But I must say, the new M4 Mac Minis' are something I would look up for, but still, getting a Mac is not my financial priority now. I got myself a RISC-V board instead lol.

## Parallel game loop
A design choice of mine is to make the engine fully parallelized with fibers, to the point that the game loop can calculate a few frames at a time - 
it should be optional tho, cause sure it massively increases bandwith, but it introduces input latency and can raise CPU temperatures. 
None of that should matter on 1) either good hardware, or 2) me making real optimizations instead of slacking off :3.

The game loop's parallel execution is divided into:
- **Simulation (frame N)** that updates the game state, physics, animations, and prepares data to be rendered.
- **Rendering (frame N-1)** is GPU-accelerated with Vulkan, to draw and audit the 3D scene, game world and interfaces.
- **GPU-execution (frame N-2)** is where the recorded command buffers are queued for work.

It forms a pipeline of parallel execution. 
When a stage finishes, it passes it's work of the current frame to the next stage, then begins working on the next frame.
There are lock-less synchronization primitives between the stages, ensuring that no stage goes too far ahead with it's work, 
and that it actually helps with executing work of other stages it is waiting for - 
all worker threads are at all times doing some work, switching the fiber context when they would wait otherwise.

This choice lead to me exploring Vulkan and ditching OpenGL (where a major design flaw would prevent me 
from doing most of the stuff from threads other than the one specific 'rendering' thread, ugh),
and to me learning some assembly, lock-less programming and other stuff I still don't really understand. 
I must keep the grind tho, even if I feel like a shitass who doesn't know what he's doing - I'll figure things out eventually, 
and would continue to cast spells *still without knowing what I'm doing*, but this time, oh this time, *with confidence*!

