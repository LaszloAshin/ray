---
layout: post
title:  "Where it all began"
date:   2022-03-19 16:19:22 +0100
categories: jekyll update
---
Writing a raytracer was a school homework excersise back in 2007.
For this project I didn't use source control so I don't have history
data. I think it was a one source file project when I submitted it.

In 2008 improvements were made and I split the code into more source files.
In 2013 I [imported them to git][first import].
This version had some issues:

- it contained an unfinished Kd-Tree implementation,
- there was a lot of dead code,
- it was very slow.

Let's see some metrics. To get this initial code [df76ccf][first import]
link I needed to modify the Makefile like this:

```diff
@@ -39,7 +39,7 @@ $(BUILDDIR)/%.o: %.cc
 -include $(ray_OBJS:%.o=$(DEPDIR)/%.d)

 ray: $(ray_OBJS:%.o=$(BUILDDIR)/%.o)
-       @$(CC) $(LDFLAGS) -o $@ $^
+       @$(CC) $^ $(LDFLAGS) -o $@
        @echo "  LN    $@"

 clean:
```

Using `gcc-10.2.1` it compiles to a `56936` byte x86\_64 executable on
linux. This is the unstripped size.

```
$ size ray
   text	   data	    bss	    dec	    hex	filename
  35323	   2056	    624	  38003	   9473	ray
```

The program needs about 14 seconds on my machine to render the 1280x800
output image:

```
$ time ./ray
62.60% Output has been written to tracement.tga
94.40% Output has been written to tracement.tga
freeNode(0)
./ray  52.22s user 0.06s system 372% cpu 14.026 total
```

Here is the result rendering:

![result rendering]({{site.baseurl}}/assets/2022-03-19-tracement.tga.png)

So that's where I started from.

The initial goal was to see if I can speed the thing up.

[first import]: https://github.com/LaszloAshin/ray/commit/df76ccff197d94574bdcf75941a7571c0ab57db6
