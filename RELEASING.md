# Releasing snek

Here are the steps to perform when releasing a new version of snek:

 1. Make sure the code builds and runs on all target hardware.

 2. Check test suite results:

	$ make check

 3. Check python code format

	$ make black

 4. Update Supported Hardware and Recent Changes sections in README.md

 5. Update SNEK_VERSION and SNEK_DATE in snek.defs

 6. Use the 'build-snek' script to compile for all target
    machines. This script uses additional software and hardware to
    create bits for machines not directly supported in Debian:

	$ ./build-snek otheros
  
 7. Commit version change:

	$ git commit -m'Snek version <version' snek.defs

 8. Tag release

	$ git tag -m'Snek version <version>' <version> main

 9. Push tag and branch to repositories

	$ git push origin main <version>
	$ git push github main <version>

 10. Publish release bits

	$ ./build-snek upload

 11. Send mail to snek mailing list including Recent Changes section
     from README.
