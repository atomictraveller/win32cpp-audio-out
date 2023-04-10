# win32cpp-audio-out

years ago this method was imparted to me for opening an audio out buffer in win32 using a minimal number of native APIs. i was never able to successfully align it with incoming audio buffers for live processing but it does turn a win32 app into a handy platform for synthesis or editing.

probably for most people, using whatever resources people who like resources use today would be good instead. using bare bones native API is such a teh forbiddenz you can basically grow old and die before you ever find out how to do things .. at least over last quarter century of observation..

apart from a few adjustments to the .cpp document, you will note the audio.h header file (and a few variables in the .h document to synthesize a loud sinewave..)
