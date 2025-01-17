project('seticore', ['cpp', 'c', 'cuda'],
        default_options: [
            'buildtype=release',
            'cpp_std=c++14',
            'werror=true',
        ])

basic_cuda_args = [ '--std=c++14' ]
newer_cuda_args = [ '--display-error-number',
                    '--diag-suppress=1675' ]

nvcc_release = run_command('./nvcc_release.sh', check: true).stdout().strip()
if nvcc_release <= '11.0'
    cuda_args = basic_cuda_args
else
    cuda_args = basic_cuda_args + newer_cuda_args
endif
    
cmake = import('cmake')

fmt_subproj = cmake.subproject('fmt')
fmt_dep = fmt_subproj.dependency('fmt')

boost_dep = dependency('boost', modules: [
    'filesystem',
    'program_options',
    'system',
])

hdf5_dep = dependency('hdf5', language: 'c')

cuda_dep = dependency('cuda', version: '>=11', modules: ['cublas', 'cufft'])

capnp_opt = cmake.subproject_options()
capnp_opt.set_override_option('warning_level', '0')
capnp_subproj = cmake.subproject('capnproto', options: capnp_opt)
kj_dep = capnp_subproj.dependency('kj')
capnp_dep = capnp_subproj.dependency('capnp')

deps = [fmt_dep, boost_dep, hdf5_dep, cuda_dep, kj_dep, capnp_dep]

srcs = [
    'beamformer.cu',
    'beamforming_config.cpp',
    'cuda_util.cu',
    'dedoppler.cu',
    'dedoppler_hit.cpp',
    'device_raw_buffer.cu',
    'dat_file_writer.cpp',
    'filterbank_buffer.cu',
    'filterbank_file_reader.cpp',
    'filterbank_metadata.cpp',
    'fil_reader.cpp',
    'h5_reader.cpp',
    'h5_writer.cpp',
    'hit.capnp.c++',
    'hit_file_writer.cpp',
    'hit_recorder.cpp',
    'multibeam_buffer.cu',
    'raw_buffer.cu',
    'raw_file.cpp',
    'raw_file_group.cpp',
    'raw_file_group_reader.cpp',
    'recipe_file.cpp',
    'run_dedoppler.cpp',
    'taylor.cu',
    'thread_util.cpp',
    'util.cpp',
    'vector_hit_recorder.cpp',
]

tests = [
    'beamformer_test.cpp',
    'dedoppler_test.cpp',
    'h5_test.cpp',
    'multibeam_buffer_test.cpp',
    'taylor_test.cu',
]

# The main binaries

executable('seticore', srcs + ['main.cpp'],
           dependencies: deps, cuda_args: cuda_args)

executable('tests', srcs + tests + ['tests.cpp'],
           dependencies: deps, cuda_args: cuda_args)

# Other binaries

executable('beamforming_integration_test',
           srcs + ['beamforming_integration_test.cpp'],
           dependencies: deps, cuda_args: cuda_args)

executable('taylor_benchmark',
           srcs + ['taylor_benchmark.cu'],
           dependencies: deps, cuda_args: cuda_args)

executable('file_io_benchmark',
           srcs + ['file_io_benchmark.cpp'],
           dependencies: deps, cuda_args: cuda_args)

executable('rawls',
           srcs + ['rawls.cpp'],
           dependencies: deps, cuda_args: cuda_args)
