;; cursed-trees --- tree evolution simulation -*- lexical binding: t -*-

;;; Commentary:

;;; Code:

(defvar cursed-trees/field nil
  "Vector with game's cells.")

(defvar cursed-trees/current-pos nil
  "Current camera's position.")

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
  (define-key cursed-trees/mode-map (kbd "<SPC>") 'cursed-trees/skip)
  (define-key cursed-trees/mode-map (kbd "f") 'cursed-trees/move-right)
  (define-key cursed-trees/mode-map (kbd "b") 'cursed-trees/move-left)
  (define-key cursed-trees/mode-map (kbd "p") 'cursed-trees/move-up)
  (define-key cursed-trees/mode-map (kbd "n") 'cursed-trees/move-down)
  (setq mode-line-format (list
						  '(:eval (format "Year:[%5d] Pos:[%3d %3d]"
										  (cursed-trees/current-year)
										  (car cursed-trees/current-pos)
										  (cadr cursed-trees/current-pos)))))
  (add-hook 'kill-buffer-hook 'cursed-trees/destroy-world)
  (add-hook 'kill-emacs-hook 'cursed-trees/destroy-world))

(defun cursed-trees/screen-width ()
  "Return width of buffer `cursed-trees/buffer-name'."
  (window-width (get-buffer-window cursed-trees/buffer-name)))

(defun cursed-trees/screen-height ()
  "Return width of buffer `cursed-trees/buffer-name'."
  (- (window-height (get-buffer-window cursed-trees/buffer-name)) 9))

(defun cursed-trees/skip (&optional years)
  "Skip YEARS in simulation.
If YEARS is nil than skip 1 year."
  (interactive)
  (if years
	  (cursed-trees/tick years)
	(cursed-trees/tick))
  (cursed-trees/display)
  (force-mode-line-update))

(defun cursed-trees/move (dx dy)
  "Move screen DX squares right and DY squares up."
  (setq cursed-trees/current-pos (cursed-trees/scroll dx dy))
  (cursed-trees/display)
  (force-mode-line-update))

(defun cursed-trees/move-right ()
  "Move screen 1 square right."
  (interactive)
  (cursed-trees/move 1 0))

(defun cursed-trees/move-left ()
  "Move screen 1 square left."
  (interactive)
  (cursed-trees/move -1 0))

(defun cursed-trees/move-up ()
  "Move screen 1 square up."
  (interactive)
  (cursed-trees/move 0 1))

(defun cursed-trees/move-down ()
  "Move screen 1 square down."
  (interactive)
  (cursed-trees/move 0 -1))

(defun cursed-trees ()
  "Launch tree simulation."
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
