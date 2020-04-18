# Project Sparrow - Captain's Log

## April 18, 2020

I'm currently struggle to wrap my head around live code reloading, and I'm still frustrated with it. This partly due to the fact that I have a longer game in my mind: a module system, which would treat any dll as an equal citizen. The idea is the following:

- Upon startup, the program inspects every dll in its module directory and tries to load its main functions (currently `Update` and `Render`)
- There would be an "item shop" struct for each module where the module can specify its needs (for example, some memory). There would also be some shared resources which each module can use (like the overall Game State).
- During the main loop, each module's main functions would be called in sequence.

This presents a number of challenges that will need to be addressed at some point.

- While I'm generally not concerned about security, I do recognize that passing around opaque dlls is not exactly one's definition of best practice and would not favour healthy modding environment. So if I want to really jump down that road I'll need to devise a better system (maybe by allowing C# scripts, config files and the other bonanza)
- Execution order will also matter at some point. Relying on files being sorted alphabetically is something I'd like to avoid.