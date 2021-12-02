# embind_threading
Example of an issue with embinding multithreaded code

## build
`cd build && emcmake cmake -DCMAKE_SYSTEM_PROCESSOR=emscripten .. && emmake make example_wasm && cd ..`

## run
`cd artifacts/served && python serve.py`
The page is then served on localhost:8000/page_example.html

## usage
The page has 3 buttons: “no threads”, “flat”, and “nested”.
Each button runs the corresponding processing: without threads, with one level of threads spawning (N_VALUES variable in example.cpp defines the number of threads, currently is 6), and with nested threads spawning (N_IMMEDIATE_THREADS threads spawn N_NESTED_THREADS threads each, currently 2 and 3 correspondingly). The worker pool size is set to POOL_SIZE cmake variable, currently 12.
All output is done to the browser console.

## issue
“no threads” and “flat” processing give expected results. Output shows initial values of two arrays of numbers, then it shows modified values when each array is processed, and then it gives the final result value.
The actual processing is irrelevant to the issue, it’s just some mock number-crunching.
“nested” processing shows initial values, then it shows modified values when the first array is processed, and then it shows the warning that says
`Tried to spawn a new thread, but the thread pool is exhausted`,
and never finishes execution.
However, the total number of spawned threads should never exceed the pool size by design of the example.

The example initializes the WASM module from a web-worker; without the worker (doing the same from the main browser thread) the same issue is observed, but it also blocks the main thread which is not convenient.
