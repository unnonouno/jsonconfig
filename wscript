import Options

VERSION='0.0.1'
APPNAME='jsonconfig'

def options(opt):
  opt.load('compiler_cxx')
  opt.load('unittest_gtest')

  opt.add_option('--enable-gcov',
                 action='store_true',
                 default=False,
                 dest='gcov',
                 help='only for debug')

def configure(conf):
  conf.env.CXXFLAGS += ['-O2', '-Wall', '-g', '-pipe']
  conf.load('compiler_cxx')
  conf.load('unittest_gtest')
  conf.check_cfg(package = 'pficommon', args = '--cflags --libs')

  if Options.options.gcov:
    conf.env.append_value('CXXFLAGS', '-fprofile-arcs')
    conf.env.append_value('CXXFLAGS', '-ftest-coverage')
    conf.env.append_value('LINKFLAGS', '-lgcov')
    conf.env.append_value('LINKFLAGS', '-fprofile-arcs')
    conf.env.append_value('LINKFLAGS', '-ftest-coverage')

def build(bld):
  bld.recurse('src')

  bld.program(
    source = 'sample/sample.cpp',
    target = 'jsonconfig_sample',
    includes = 'src',
    use = 'jsonconfig'
    )

def cpplint(ctx):
  cpplint_args = '--filter=-runtime/references,-legal/copyright'

  src_dir = ctx.path.find_node('src')
  files = []
  for f in src_dir.ant_glob('**/*.cpp **/*.hpp'):
    files.append(f.path_from(ctx.path))

  args = 'cpplint.py %s %s' % (cpplint_args,' '.join(files))
  result = ctx.exec_command(args)
  if result != 0:
    ctx.fatal('cpplint failed')
