# ray_maze

https://user-images.githubusercontent.com/13428215/165850055-bb50f288-5bc3-42e1-bf5f-395abac7eb22.mp4

## What is this?

This is a little maze generator written in C. It uses the [`raylib`](https://raylib.com) and [`raygui`](https://github.com/raysan5/raygui) to draw graphics. It also uses the C standard library, as many C programs do. The rest of the code is completely handwritten.

## Why did you make this?

I really like mazes. [I'm not alone in this](https://weblog.jamisbuck.org/2010/12/27/maze-generation-recursive-backtracking), I swear! I recently learned C and realized that finally I knew enough to use `raylib` to make something actually interesting. I knew that maze generation tends to be a pretty straightforward project from past experience, and figured that it would be a nice challenge to do it in C. I was right on both counts!

## How did you make this?

The [first prototype](https://gist.github.com/nok-ko/1c67549ddc77267e94200826b787a865) had ANSI terminal graphics. I was able to split up the terminal display code and the maze-generation code fairly easily – the maze generator was more-or-less unmodified for the first iteration of the GUI.

The text-only generator creates a maze in one shot: call `carve_maze()`, boom, you have a maze. I wanted something a little fancier, and figured it would look cool if I could watch the algorithm carve a really large maze, one cell at a time. I could've used something like [`curses`](https://en.wikipedia.org/wiki/Curses_(programming_library)) to make animations in the terminal, but I figured that without using [special characters](https://github.com/asciimoo/drawille), the resolution would be quite bad. So I wanted to write a proper frontend for it.

https://user-images.githubusercontent.com/13428215/165842592-d89ba5dd-cc81-4c17-9827-05236ddbb7e8.mp4

Pictured above: The first GUI prototype.

The recursive `carve()` function in the original program poses a problem: To animate it, I would need to introduce a delay between every recursive call to `carve()`. 

Ideally, I would stop the function mid-execution, draw what it had carved so far, then resume execution. The usual primitive for this kind of thing is a [coroutine](https://en.wikipedia.org/wiki/Coroutine), and while using them would have been quite elegant, C doesn't exactly have easy-to-use built-in coroutines. (Although [some people](https://www.chiark.greenend.org.uk/~sgtatham/) have written some [truly cursed macros](https://www.chiark.greenend.org.uk/~sgtatham/coroutines.html) to mimic them.) 

Instead the solution I came to was to make an explicit stack, separate from the call stack, and rewrite `carve()` to do one step of the algorithm and alter the stack after it finished. The beautiful, recursive `carve()` becomes a slightly uglier, iterative `carve_iter()`, which needs a reference to the maze data, and he aforementioned stack to do its dark work. 

Then, we simply call `carve_iter()` as needed in every draw call: Keep a frame counter `frames`, incrementing it every `draw()`, then call `carve_iter()` when `frames` is a multiple of 3, or some other factor. (Higher factors make for slower maze generation.) Animation: achieved!

https://user-images.githubusercontent.com/13428215/165843787-50a5ca53-cd45-4863-b094-fa7badf4a581.mp4

Then I jazzed up the program a little bit, adding GUI elements with `raygui` to control the speed of the generation, and to reset the generator's progress:

https://user-images.githubusercontent.com/13428215/165850222-f3bf26a8-374c-4a2f-8619-83540977dee1.mp4

(It also goes through a user-defined colour gradient as it carves! `:D`)

## Final Round: the .MAZ file format!

I also decided to make my own file format, mostly for fun, and also for some semi-practical reasons. [Here's a (somewhat) detailed description of the .MAZ file format.](https://nok-ko.github.io/ray_maze/maz_format.html)

Because I can't resist learning cute DSLs, I also learned enough [Tcl](https://tcl.tk) to write a [Hex Fiend](https://github.com/HexFiend/HexFiend) [Binary Template](https://github.com/HexFiend/HexFiend/blob/master/templates/Tutorial.md), which let me pretty-print the `.MAZ` data in my favourite hex editor. Find it in [`other/MAZ.tcl`](https://github.com/nok-ko/ray_maze/blob/main/other/MAZ.tcl) in this repository.

## Special Thanks

* Jamis Buck, for writing the best maze-related content on the web.
* My friends, for putting up with weeks of my maze generation exploits, and cheerleading for me.
* The [ASAN](https://clang.llvm.org/docs/AddressSanitizer.html) team, for releasing a tool that almost makes me feel like I'm writing [memory-safe](https://en.wikipedia.org/wiki/Memory_safety) C.
