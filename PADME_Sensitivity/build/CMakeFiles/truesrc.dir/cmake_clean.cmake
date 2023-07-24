FILE(REMOVE_RECURSE
  "libtruesrc.pdb"
  "libtruesrc.so"
)

# Per-language clean rules from dependency scanning.
FOREACH(lang)
  INCLUDE(CMakeFiles/truesrc.dir/cmake_clean_${lang}.cmake OPTIONAL)
ENDFOREACH(lang)
