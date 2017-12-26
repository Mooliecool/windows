(defun Length (LIST) (if (null LIST)
                         0
                         (+ 1 (Length (cdr LIST)))))

(defun Remove (L e) 
  (do ((lst L (cdr lst))
       (c (if (= (car lst) (car e)) 'NIL (car lst))
	      (if (= (car lst) (car e)) c (cons c (car lst)))))
      ((null lst) c)
      ((= (car lst) (car e)) 
       (cons c (cdr lst)))))

(defun Sort (X) 
  (do ((lst X (Remove lst (do ((lst1 lst (cdr lst1))
			   (min (car lst) (if (< (car min) (car lst1)) min (car lst1))))
			  ((null (cdr lst1))
			   min))))
       (count (Length X) (- count 1))
       (Srt 'NIL (cons Srt min)))
      ((= count 0)
       Srt)))

		     
(Sort '(5 1 10))

(Sort '(-11 8 15 67 19 11 10 9 8 7 1 100 200 300 6 5 0 4 3 2 1 0))

