(lang dune 3.3)

(name shine)

(generate_opam_files true)

(source
 (github EruEri/shine))

(authors "EruEri")

(maintainers "EruEri")

(license LGPLv2.1)

(package
 (name shine)
 (synopsis "A small ruby project creator")
 (depends ocaml dune (clap (>= :"0.2.0"))))

; See the complete stanza docs at https://dune.readthedocs.io/en/stable/dune-files.html#dune-project
