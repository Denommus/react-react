{ pkgs ? import <nixpkgs> {} }:
with pkgs;
let
  ocaml_4_06 = callPackage <nixpkgs/pkgs/development/compilers/ocaml/4.06.nix> {};
  ocamlPackages' = ocamlPackages.overrideScope' (self: super: {
    ocaml = ocaml_4_06;
  });
in
mkShell {
  buildInputs = [
    ocaml_4_06
    nodejs
    python
    stdenv
    git
    nodePackages.ocaml-language-server
    ocamlPackages'.merlin
    emacs
    ocamlPackages'.reason
  ];
}
