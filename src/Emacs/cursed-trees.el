;;; cursed-trees --- tree evolution simulation       -*- lexical binding: t; -*-

;;; Copyright 2021 Adil Mokhammad

;; This program is free software; you can redistribute it and/or modify
;; it under the terms of the GNU General Public License as published by
;; the Free Software Foundation, either version 3 of the License, or
;; (at your option) any later version.

;; This program is distributed in the hope that it will be useful,
;; but WITHOUT ANY WARRANTY; without even the implied warranty of
;; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
;; GNU General Public License for more details.

;; You should have received a copy of the GNU General Public License
;; along with this program.  If not, see <http://www.gnu.org/licenses/>.

;;; Commentary:

;; Tree evolution simulation.  The main part of this program is written in C++ so you need build cursed-trees-module.
;; Trees evolve by itself just watch how they survive.
;; To run the program add cursed-trees-module.so and this file to your load path.  Then eval (require 'cursed-trees) and type M-x cursed-trees.
;; You can move camera by 'p', 'b', 'n', 'f'.
;; Skip 100 years by typing 's'.
;; Skip 1000 years by typing 'S'.
;; Toggle energy mode by typing 'e'.
;; Type +/- to increase/decrease sun's energy.

;;; Code:

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; Internal variables ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

(defvar cursed-trees/field nil
  "Vector with game's cells.")

(defvar cursed-trees/current-pos nil
  "Current camera's position.")

(defvar cursed-trees/timer nil
  "Timer for `cursed-trees'.")


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; Customizations ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

(defgroup cursed-trees nil
  "`cursed-trees' - tree evolution simulation."
  :prefix "cursed-trees/"
  :group 'games
  :link "https://github.com/LLLida/cursed-trees/tree/emacs-module")

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


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; Private functions ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

(defun cursed-trees/screen-width ()
  "Return width of buffer `cursed-trees/buffer-name'."
  (window-width (get-buffer-window cursed-trees/buffer-name)))

(defun cursed-trees/screen-height ()
  "Return width of buffer `cursed-trees/buffer-name'."
  (- (window-height (get-buffer-window cursed-trees/buffer-name)) 5))

(defun cursed-trees/move (dx dy)
  "Move screen DX squares right and DY squares up."
  (let ((old-pos cursed-trees/current-pos))
	(setq cursed-trees/current-pos (cursed-trees/scroll dx dy))
	(when (and old-pos ;; check if (= old-pos current-pos) only when old-pos is non-nil
			   (= (car cursed-trees/current-pos) (car old-pos))
			   (= (cadr cursed-trees/current-pos) (cadr old-pos)))
	  (message "Can't move")))
  (cursed-trees/display)
  (force-mode-line-update))

(defun cursed-trees/create-field ()
  "Create vector containing cells for rendering."
  (interactive)
  (setq cursed-trees/field (make-vector (* (cursed-trees/screen-width)
										   (cursed-trees/screen-height))
										?\.)))

(defun cursed-trees/init ()
  "Init `cursed-trees'."
  (interactive)
  (cursed-trees/create-field)
  (require 'cursed-trees-module)
  (cursed-trees/create-world))

(defun cursed-trees/display ()
  "Display the game."
  (interactive)
  (with-current-buffer (get-buffer cursed-trees/buffer-name)
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
	  (goto-char pos))))

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


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; Public functions ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

(define-derived-mode cursed-trees/mode special-mode "cursed-trees"
  (setq-local scroll-margin 0 ;; prevent user from scrolling the screen
			  )
  ;; Keybindings
  (define-key cursed-trees/mode-map (kbd "<SPC>") 'cursed-trees/skip)
  (define-key cursed-trees/mode-map (kbd "f") 'cursed-trees/move-right)
  (define-key cursed-trees/mode-map (kbd "<right>") 'cursed-trees/move-right)
  (define-key cursed-trees/mode-map (kbd "b") 'cursed-trees/move-left)
  (define-key cursed-trees/mode-map (kbd "<left>") 'cursed-trees/move-left)
  (define-key cursed-trees/mode-map (kbd "p") 'cursed-trees/move-up)
  (define-key cursed-trees/mode-map (kbd "<up>") 'cursed-trees/move-up)
  (define-key cursed-trees/mode-map (kbd "n") 'cursed-trees/move-down)
  (define-key cursed-trees/mode-map (kbd "<down>") 'cursed-trees/move-down)
  (define-key cursed-trees/mode-map (kbd "e") 'cursed-trees/toggle-energy-mode)
  (define-key cursed-trees/mode-map (kbd "+") 'cursed-trees/increase-sun-energy)
  (define-key cursed-trees/mode-map (kbd "-") 'cursed-trees/decrease-sun-energy)
  (define-key cursed-trees/mode-map (kbd "s") (lambda ()
                                                "Skip 100 years."
                                                (interactive)
                                                (cursed-trees/skip 100)))
  (define-key cursed-trees/mode-map (kbd "S") (lambda ()
                                                "Skip 1000 years."
                                                (interactive)
                                                (cursed-trees/skip 1000)))
  ;; Modeline
  (setq mode-line-format (list
						  '(:eval (format "Year:[%5d] Pos:[%3d %3d] Trees:[%4d]"
										  (cursed-trees/current-year)
										  (car cursed-trees/current-pos)
										  (cadr cursed-trees/current-pos)
										  (cursed-trees/num-trees)))))
  ;; Destroy world and kill timer when buffer is closed
  (setq-local kill-buffer-hook (lambda ()
                                 (cursed-trees/destroy-world)
                                 (when cursed-trees/timer
                                   (cancel-timer cursed-trees/timer)
                                   (setq cursed-trees/timer nil)))
              ;; Recreate field when resizing
              window-size-change-functions (list (lambda (&rest ignore)
                                                   (cursed-trees/create-field))))
  ;; Emacs will crash if we won't destroy module's global variables by hand
  (add-hook 'kill-emacs-hook 'cursed-trees/destroy-world))

(defun cursed-trees ()
  "Launch tree simulation."
  (interactive)
  (when cursed-trees/timer
    (cancel-timer cursed-trees/timer)
    (setq cursed-trees/timer nil))
  (require 'cursed-trees-module)
  (setq cursed-trees/timer
        (run-at-time nil 0.5 #'cursed-trees/skip))
  (switch-to-buffer cursed-trees/buffer-name)
  (cursed-trees/init)
  (cursed-trees/mode)
  (cursed-trees/display))

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

(defun cursed-trees/increase-sun-energy ()
  "Increment `cursed-trees/sun-energy' by 1."
  (interactive)
  (message "Sun's energy is now %d"
		   (setq cursed-trees/sun-energy (1+ cursed-trees/sun-energy))))

(defun cursed-trees/decrease-sun-energy ()
  "Decrement `cursed-trees/sun-energy' by 1."
  (interactive)
  (message "Sun's energy is now %d"
		   (setq cursed-trees/sun-energy (1- cursed-trees/sun-energy))))

(defun cursed-trees/skip (&optional years)
  "Skip YEARS in simulation.
If YEARS is nil than skip 1 year."
  (interactive)
  (let ((ticks (cursed-trees/tick years)))
    (cond ((= ticks 0)
           (message "No life."))
          ((and years (> years 1)) ;; years may be nil
           (message "Skipped %d years." years))))
  (cursed-trees/display)
  (force-mode-line-update))

(defun cursed-trees/toggle-energy-mode ()
  "Toggle `cursed-trees/energy-mode'.
Return value of `cursed-trees/energy-mode'."
  (interactive)
  (setq cursed-trees/energy-mode (not cursed-trees/energy-mode))
  (if cursed-trees/energy-mode
	  (message "Energy mode enabled.")
	(message "Energy mode disabled."))
  (cursed-trees/display))

(provide 'cursed-trees)
;;; cursed-trees.el ends here
