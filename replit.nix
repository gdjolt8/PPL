{ pkgs }: {
	deps = [
		pkgs.nasm
  pkgs.strace
  pkgs.valgrind
  pkgs.clang_12
		pkgs.ccls
		pkgs.gdb
		pkgs.gnumake
	];
}