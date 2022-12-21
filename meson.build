project('kommunikationsmodul', 'cpp')

src = files(
    'src/main.cpp',
    'src/binarization.cpp',
    'src/line.cpp',
    'src/perspective.cpp'
)

fmt_dep = dependency('fmt')
thread_dep = dependency('threads')
opencv_dep = dependency('opencv4')

executable('lane-detection', src,
           dependencies: [fmt_dep, thread_dep, opencv_dep],
           cpp_args: ['-std=c++17'])

