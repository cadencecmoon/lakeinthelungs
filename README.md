# Lake in the Lungs
I'm a CS student from Poland, just having fun with this gamedev-related project of mine.
My goal is to make a 3D raytracer in Vulkan, and if things work out, to fully flesh out an actual video game from this.
The *A Moonlit Walk Engine* is part of this project, and it's optimized (or limited I guess) to the stuff I need 
(not true, I *can't stop myself* from not implementing more than I need).

The engine is open. Feel free to take my code, change stuff, use it in your own applications/video games.
It's published under a MIT license.

There won't be art assets in this repository, and if I'll publish any, they will be licensed in different terms.

I use C99 and assembly, for now targetting 64-bit platforms like x86\_64, ARM and RISC-V, but I'm trying to make it atleast somewhat compatible with 32-bit CPUs.
Dependencies are kept to a minimum, with most API libraries being loaded at runtime. 
Those dependencies are still needed for a complete build, to make use of SDK's, headers, etc.

Vulkan is my rendering API of choice for this project. 
I'm not sure i'll implement other rendering backends, like DirectX12, Metal or WebGPU (not even mentioning the proprietary stuff), that's not really important now.
But I still want to make sure that the main platforms I'm interested in will be supported, or that porting to a new platform will be very much straighforward.

Oh, and today as I'm writing, Vulkan 1.4 was released. So I'll take that into account when implementing the renderer from scratch. 
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

I promise I will dual boot Windows on my laptop *eventually*, so I can actually port this engine to Windows.
And for MacOS, I must say, I don't own an Apple device. 
So any effort I make for implementing the MACH-O ABI assembly, Cocoa, CoreAudio or some another OBJ-C specific API, it's futile without any way to test this code.
But I must say, the new M4 Mac Minis' are something I would look up for, but still, getting a Mac is not my financial priority now. I got myself a RISC-V board instead lol.

## Parallel game loop
A design choice of mine is to make the engine fully parallelized with fibers, to the point that the game loop can calculate a few frames at a time - 
it should be optional tho, cause sure it massively increases bandwith, but it introduces input latency and is not very power-friendly for the CPU. 
None of that should matter on 1) either good hardware, 2) a machine dedicated to run games, with not many stuff done in the background, or 3) me making real optimizations instead of slacking off :3.

The game loop's parallel execution is divided into:
- **Simulation** that updates the game state, physics, animations, and prepares data to be rendered.
- **Rendering** to draw and audit the 3D scene, game world and interfaces, records command buffers.
- **GPU-execution** is where the recorded command buffers are queued for work in the GPU.

When a stage finishes, it passes it's work of the current frame to the next stage, then begins working on the next frame.
The stages are synchronized, making sure that the pipeline continues only if all the current work has been completed, ensuring that no stage goes too far with it's work.

This choice lead to me exploring Vulkan, ditching OpenGL (yeah, there will be no OpenGL support in this ever, Vulkan is enough tho)
and to me learning some assembly, lock-less programming and other stuff I still don't really understand *yet*. 
I must keep the grind tho, even if I feel like a shitass who doesn't know what he's doing - I'll figure things out eventually.

