Building custom controls in wxWidgets, drawing on canvas and using affine transforms to rotate, scale and move objects. Mouse interactions, custom events and much more! See the [video tutorial](https://www.youtube.com/watch?v=sCTaMkbnup0) for details.

[![Video](/output.gif)](https://www.youtube.com/watch?v=sCTaMkbnup0)

### Building

To build the project, use:

```bash
cmake -S. -Bbuild
cmake --build build
```

This will create a directory named `build` and create all build artifacts there. The main executable can be found in the `build/subprojects/Build/wx_drawing_tutorial_core` folder.

### Reqiurements

If you don't have wxWidgets installed, the CMake script will download and build the dependencies automatically.

But if CMake script finds wxWidgets on your system, make sure it's at least version 3.1.0.

---
Check out the blog for more! [www.onlyfastcode.com](https://www.onlyfastcode.com)
---
