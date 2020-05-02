# Project Sparrow - Captain's Log

## April 18, 2020

I'm currently struggle to wrap my head around live code reloading, and I'm still frustrated with it. This partly due to the fact that I have a longer game in my mind: a module system, which would treat any dll as an equal citizen. The idea is the following:

- Upon startup, the program inspects every dll in its module directory and tries to load its main functions (currently `Update` and `Render`)
- There would be an "item shop" struct for each module where the module can specify its needs (for example, some memory). There would also be some shared resources which each module can use (like the overall Game State).
- During the main loop, each module's main functions would be called in sequence.

This presents a number of challenges that will need to be addressed at some point.

- While I'm generally not concerned about security, I do recognize that passing around opaque dlls is not exactly one's definition of best practice and would not favour healthy modding environment. So if I want to really jump down that road I'll need to devise a better system (maybe by allowing C# scripts, config files and the other bonanza)
- Execution order will also matter at some point. Relying on files being sorted alphabetically is something I'd like to avoid.

## April 22, 2020

I now think that there most probably should be three levels for the engine.

- Platform layer would take care of the interaction with the operating system: securing memory, allocating memory, receiving input, etc., packaging it all and passing to the engine proper.
- Engine layer would take care of the most basic systems. Rendering core, whatever gameobject system I come up with, etc. Core systems which would unify the modules and prevent to reimplement any mod.
- Mod layer would kick in (and use) engine resources. I have no idea how this would be implemented, but I imagine it would be scripted on the fly (while using all the benefits of live reloading etc.). The latter is the fuzziest of all, and I can think of at least few huge questions:

- I'm not sure whether I need a full scripting system (i.e. not using compilation step), runtime recompilation (a-la QuakeC) or just allow people to compile their own module. In the latter case, would we allow IL mods (C#) or pure machine code will be required?
- I really don't want to load DLLs from the engine, that would mean having to pass chunks of the engine back to the platform layer, providing it to the modules and THEN executing them.

As an alternative, I can split between Audio/Video render systems and Game Update. The latter can be expanded, while the former would only be used from a single DLL.

Questions, questions... It's still too early to arrive to the answers. I'll need to implement the rendering system and basic loops and figure the rest out later.

Anyway, by now we have hot code reloading for at least one module. I can start playing with the rendering systems and see what sticks.

Just keep thinking about that modularity.

## May 2, 2020

We returned back to the land of Windows platform to tackle one of the big guys: the beginnings of the input system. Turns out, most of the work was **kinda** done, but it's also not exactly true. There's still refinment to be done, especially if we want to increase our control over the input we're getting.

Right now, we only process windows messages. So, if Windows in its infinite wisdom decides that we want to know about the state of a key input, it will say so, but not otherwise. This way we have this beautiful ~1s delay between a key hit and repeated key hits. In any input system that's not about typing this is a no-go.

But, this is something we can easily handle later down the line. For now, we can send some basic commands down to the system, so our first demo project can take place.

One thing that came to me regarding the potential modularity is completely separating renderer away. So it would be done opaquely in an "core" module, while the "mods" would be only responsible of implementing their own GameUpdate function. There might also be a centralized "CombineGameUpdates" function in the core module which would gather the results from all the mods and try to merge them together? We'll see when we get there.
