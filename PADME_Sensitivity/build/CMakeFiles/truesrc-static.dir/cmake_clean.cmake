FILE(REMOVE_RECURSE
  "libtruesrc-static.pdb"
  "libtruesrc-static.a"
)

# Per-language clean rules from dependency scanning.
FOREACH(lang)
  INCLUDE(CMakeFiles/truesrc-static.dir/cmake_clean_${lang}.cmake OPTIONAL)
ENDFOREACH(lang)
