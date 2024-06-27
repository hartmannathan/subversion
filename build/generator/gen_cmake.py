import os
from build.generator.gen_make import UnknownDependency
import ezt
import gen_base

class _eztdata(object):
  def __init__(self, **kw):
    vars(self).update(kw)

class cmake_target():
  def __init__(self, name: str, type: str, sources, libs):
    self.name = name
    self.type = type
    self.sources = sources
    self.libs = libs

def get_target_type(target: gen_base.Target):
  if isinstance(target, gen_base.TargetExe):
    if target.install == "test" and target.testing != "skip":
      return "test"
    else:
      return "exe"
  if isinstance(target, gen_base.TargetSWIG):
    return "swig"
  if isinstance(target, gen_base.TargetSWIGProject):
    return "swig-project"
  if isinstance(target, gen_base.TargetSWIGLib):
    return "swig-lib"
  if isinstance(target, gen_base.TargetLib):
    return "lib"
  else:
    return str(type(target))

class Generator(gen_base.GeneratorBase):
  _extension_map = {
    ('exe', 'target'): '.exe',
    ('exe', 'object'): '.obj',
    ('lib', 'target'): '.dll',
    ('lib', 'object'): '.obj',
    ('pyd', 'target'): '.pyd',
    ('pyd', 'object'): '.obj',
    ('so', 'target'): '.so',
    ('so', 'object'): '.obj',
  }

  def __init__(self, fname, verfname, options=None):
    gen_base.GeneratorBase.__init__(self, fname, verfname, options)

  def write(self):
    targets = []

    for target in self.get_install_sources():
      target: gen_base.Target

      if isinstance(target, gen_base.TargetScript):
        # there is nothing to build
        continue
      elif isinstance(target, gen_base.TargetExe):
        if target.install == "test" or target.install == "sub-test":
          pass
        elif target.install == "tools":
          pass
        else:
          pass
      elif isinstance(target, gen_base.TargetRaModule):
        pass
      elif isinstance(target, gen_base.TargetFsModule):
        pass
      elif isinstance(target, gen_base.TargetApacheMod):
        pass
      elif isinstance(target, gen_base.TargetLib):
        pass

      sources = []
      libs = []

      for dep in self.get_dependecies(target.name):
        if isinstance(dep, gen_base.TargetLinked):
          if dep.external_lib:
            if dep.name == "ra-libs":
              # TODO
              pass
            elif dep.name == "fs-libs":
              # TODO
              pass
            elif dep.name in ["apriconv",
                              "apr_memcache",
                              "magic",
                              "intl",
                              "macos-plist",
                              "macos-keychain",
                              "sasl"]:
              # These dependencies are currently ignored
              # TODO:
              pass
            else:
              libs.append("external-" + dep.name)
          else:
            libs.append(dep.name)
        elif isinstance(dep, gen_base.ObjectFile):
          deps = self.graph.get_sources(gen_base.DT_OBJECT,
                                        dep,
                                        gen_base.SourceFile)
          for dep in deps:
            sources.append(dep.filename)

      target_type = get_target_type(target)

      target = cmake_target(
        name = target.name,
        type = target_type,
        sources = sources,
        libs = libs
      )

      if target_type in ["exe", "lib"]:
        targets.append(target)

    data = _eztdata(
      targets = targets,
    )

    template = ezt.Template(os.path.join('build', 'generator', 'templates',
                                         'CMakeLists.txt.ezt'),
                            compress_whitespace=False)
    template.generate(open('CMakeLists.txt', 'w'), data)

  def get_install_sources(self):
    install_sources = self.graph.get_all_sources(gen_base.DT_INSTALL)
    result = []

    for target in install_sources:
      target: gen_base.Target

      if not self.check_ignore_target(target):
        result.append(target)

    result.sort(key = lambda s: s.name)

    return result

  def get_dependecies(self, target_name):
    deps = []

    deps += self.graph.get_sources(gen_base.DT_LINK, target_name)
    deps += self.graph.get_sources(gen_base.DT_NONLIB, target_name)

    return deps

  def check_ignore_target(self, target: gen_base.Target):
    ignore_names = [
      "libsvn_auth_gnome_keyring",
      "libsvn_auth_kwallet",

      "libsvnxx",
      "svnxx-tests",

      "libsvn_fs_base",
      "libsvn_ra_serf",

      "mod_authz_svn",
      "mod_dav_svn",
      "mod_dontdothat",

      "libsvnjavahl",
      "__JAVAHL__",
      "__JAVAHL_TESTS__",
    ]

    for name in ignore_names:
      if target.name == name:
        return True

      if isinstance(target, gen_base.TargetExe):
        if target.install == "bdb-test":
          return True
