;; cursed-trees --- tree evolution simulation -*- lexical binding: t -*-

;;; Commentary:

;;; Code:

(defvar cursed-trees/field nil
  "Vector with game's cells.")

(defgroup cursed-trees nil
  "`cursed-trees' - tree evolution simulation."
  :prefix "cursed-trees/"
  :group 'games)

(defcustom cursed-trees/world-width 200
  "Width of world."
  :type 'integer)

(defcustom cursed-trees/world-height 60
  "Height of world."
  :type 'integer)

(defcustom cursed-trees/buffer-name "*cursed-trees*"
  "Name of buffer for game."
  :type 'string)

(defcustom cursed-trees/num-trees 10
  "Number of trees at start."
  :type 'integer)

(defcustom cursed-trees/initial-energy 300
  "Energy at start."
  :type 'integer)

(defcustom cursed-trees/sun-energy 5
  "Sun's energy."
  :type 'integer)

(defcustom cursed-trees/energy-mode nil
  "Non-nil means display trees' energy."
  :type 'boolean)

(define-derived-mode cursed-trees/mode special-mode "cursed-trees"
  (setq-local scroll-margin 0 ;; prevent user from scrolling the screen
			  )
  (define-key cursed-trees/mode-map (kbd "<SPC>") (lambda ()
													(interactive)
													(cursed-trees/tick)
													(cursed-trees/display)
													(force-mode-line-update)))
  (setq mode-line-format (list
						  '(:eval (format "Year:[%5d]" (cursed-trees/current-year)))))
  (add-hook 'kill-buffer-hook 'cursed-trees/destroy-world))

(defun cursed-trees/screen-width ()
  "Return width of buffer `cursed-trees/buffer-name'."
  (window-width (get-buffer-window cursed-trees/buffer-name)))

(defun cursed-trees/screen-height ()
  "Return width of buffer `cursed-trees/buffer-name'."
  (- (window-height (get-buffer-window cursed-trees/buffer-name)) 9))

(defun cursed-trees ()
  "Launch Emacs Game Engine."
  (interactive)
  (switch-to-buffer cursed-trees/buffer-name)
  (cursed-trees/init)
  (cursed-trees/mode)
  (cursed-trees/display))

(defun cursed-trees/init ()
  "Reinit game engine."
  (interactive)
  (setq cursed-trees/field (make-vector (* (cursed-trees/screen-width)
										   (cursed-trees/screen-height))
										?\.))
  (require 'cursed-trees-module)
  (cursed-trees/create-world))

(defun cursed-trees/display ()
  "Display the game."
  (interactive)
  (let ((inhibit-read-only t)
		(pos (point))
		(width (cursed-trees/screen-width))
		(height (cursed-trees/screen-height)))
	(erase-buffer)
	(dotimes (row height)
	  (dotimes (col width)
		(insert (cursed-trees/get-square col row)))
	  ;; insert endline only when row is not last
	  (when (/= row (1- height))
		(insert "\n")))
	(goto-char pos)))

(defun cursed-trees/get-square (x y)
  "Get square at position (X, Y)."
  (elt cursed-trees/field
	   (+ x (* y (cursed-trees/screen-width)))))

(defun cursed-trees/set-square (x y value fgcolor bgcolor)
  "Set square at position (X, Y) to VALUE with FGCOLOR and BGCOLOR."
  (aset cursed-trees/field
		(+ x (* y (cursed-trees/screen-width)))
		(propertize value
					'face (list :foreground fgcolor
								:background bgcolor))))



(provide 'cursed-trees)
;;; cursed-trees.el ends here
