# Releasing Snek

Here are the steps to take when creating a new release of snek.

 1. Make sure python code is all formatted correctly

	$ make black

 2. Make sure the current code runs on all supported targets:

  ** crickit
  ** duemilanove
  ** duemilanove-big
  ** esp32
  ** ev3
  ** feather
  ** grove
  ** hifive1revb
  ** itsybitsy3v
  ** itsybitsy5v
  ** itsybitsym0
  ** lilypad
  ** lilypad-big
  ** mega
  ** metrom0
  ** nano33iot
  ** nano-every
  ** narrow-1284
  ** playground
  ** posix (both Linux and Mac OS X)
  ** qemu-arm
  ** qemu-riscv
  ** snekboard
  ** uduino
  ** uno
  ** uno-big
  ** xiao
  ** windows

 3. Test on architectures that can run on the local host:

  ** posix
  ** qemu-arm
  ** qemu-riscv

	$ ./build-snek check

 4. Add release notes to README.md

 5. Update snek.defs with version number and build date

 6. Commit release notes and snek.defs

	$ git commit -s -m'Snek version <version>' README.md snek.defs

 7. Tag the release

	$ git tag -m'Snek version <version>' -s <version> main

 8. Build a test Debian package

	$ git checkout debian
	$ git merge <version>
	$ dch -v <version>-1
	$ git commit -s -m'debian: Snek version <version>-1' debian/changelog
	$ gbp buildpackage

 9. Build and upload release

	$ git checkout main
	$ ./build-snek clean
	$ ./build-snek upload

10. Update sneklang.org web site, replacing 'Current Status' section.

	$ emacs $HOME/misc/sneklang/index.mdwn

11. Build final Debian packge

	$ git checkout debian
	$ dch -r
	$ git commit --amend changelog
	$ git tag -m'debian: Snek version <version>-1' -s <version>-1 debian
	$ gbp buildpackage

12. Push git updates

	$ git push origin main <version> debian <version>-1

13. Send out a release announcement to the mailing list
    (snek@keithp.com)
