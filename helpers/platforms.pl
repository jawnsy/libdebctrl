#!/usr/bin/perl

use strict;
use warnings;

use Dpkg::Arch ('get_valid_arches');

my (@platforms);

# Fill in lists of operating systems and architectures
# This assumes get_valid_arches doesn't return any duplicates. If it does, the
# binary search code might be broken (but I doubt it).
foreach my $platform (get_valid_arches()) {
  my ($os, $arch) = split(/-/, $platform);
  # If arch isn't defined, then it's Linux, and $os contains the arch
  if (!defined $arch) {
    push(@platforms, 'linux-' . $os);
  }
  push(@platforms, $platform);
}

# Add special values: source, any, all (Debian Policy Sec. 5.6.8)
push(@platforms, 'source', 'any', 'all');

@platforms = sort(@platforms);

print {*STDERR} "Generating platform count for validate.h...\n";
printf "#define PLATFORM_COUNT %d\n", scalar(@platforms);

print {*STDERR} "Generating platform list for validate.c...\n";
print "static const char *platforms[PLATFORM_COUNT] = {\n";
print '  "'; # prefix for the first line
print join("\",\n  \"", @platforms);
print "\"\n};\n"; # suffix for the last line
