#!/usr/local/bin/lisp

(defvar os "linux")

(defvar arch
  (let ((uname-arch (strip ($ (sh uname -m)))))
    (cond
      ((= ("aarch64"  uname-arch)) "arm64")
      ((= ("x86_64"   uname-arch)) "amd64")
      (true "unknown"))))

(defvar current-dir (strip ($ (sh pwd))))

(defvar workspace (strcat current-dir "/build"))

(defvar release-name (strcat "lisp-" os "-" arch ".tar.gz"))

(defun build-release (build-dir target)
  (and
    (sh cmake -B build-dir)
    (sh cmake --build build-dir --parallel --target target)))

(defun bundle-relase (archive-name)
  (let ((tmp-dir (strip ($ (sh mktemp -d)))))
    (and
      (sh cp -v "build/src/lisp" tmp-dir)
      (sh cp -vr "stdlib" tmp-dir)
      (sh tar -czvf archive-name -C tmp-dir "lisp" "stdlib")
      (sh rm -rfv tmp-dir))))

(defun upload-release (source)
  (let ((server "root@www.jakobmaier.at")
        (target "/var/www/files/lisp/release/latest/")
        (upload-target (strcat server ":" target)))
    (sh scp source upload-target)))

(build-release "build" "lisp")

(bundle-relase release-name)

(upload-release release-name)

(sh rm -v release-name)

(println "Uploaded release " release-name)