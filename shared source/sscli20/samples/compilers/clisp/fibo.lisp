(defun Fib (N)
  (if (<= N 0) 
      0 
    (if (= N 1) 
	1 
      (+ (Fib (- N 1)) (Fib (- N 2))
	 )
      )
    )
  )

(defun Fibo (N) (do ((count 0 (+ count 1))
		 (Fibo (car (Fib 0)) (cons Fibo (car (Fib count)))))
		((> count N)
		 Fibo)))

(Fibo 25)





