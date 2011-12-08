Introduction
============

It basically does the same as https://github.com/tmbinc/bgrep with a few
improvements, written from the ground up.
I learned about this project after the fact I've written this and I like the name,
Therefore this project is named bgrep2, but still produces the same grep-like
utility.

bgrep is a binary grep utility, it allows you to match a binary pattern against a
binary file, and presents it in a way which is easy to understand.

So, something like the following.

    #> bgrep itself /bin/ls
    ==== 0x00015610 (0x00015610-0x00015620) ====
    00015610: 69747365 6c660a20 202d6d20 20202020    itse lf.. .-m. ....
              ^-
    ====

By default, the specified byte string is matched verbatim, by using the option '-x' 
you can instead match a hexadecimal sequence.

    #> bgrep -x deff /bin/ls
    
    ==== 0x00004777 (0x00004770-0x00004780) ====
    00004770: 6d210031 dbe80ede ffff660f 1f440000    m!.1 .... ..f. .D..
                             ^-                  
    ====
    ==== 0x00006c45 (0x00006c40-0x00006c50) ====
    00006c40: 8b3ce0e8 f8deffff 4885db49 89c30f84    .<.. .... H..I ....
                         ^-                      
    ====
    ==== 0x00008115 (0x00008110-0x00008120) ====
    00008110: 83c308e8 88deffff 4439e57f df48833d    .... .... D9.. .H.=
                         ^-                      
    ====

Installation
============

The following will install bgrep into /usr/local/bin by default.

    #> make bgrep
    #> sudo make install

License
=======

This software is covered by a BSD 2-clause license, please see LICENSE for
details.

Happy Hacking!
