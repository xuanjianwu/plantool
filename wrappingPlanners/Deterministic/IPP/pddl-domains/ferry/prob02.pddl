(define (problem ferry2)
    (:init (place a)
	   (place b)
	   (auto c1)
	   (at c1 a)
	   (at-ferry a)
	   (empty-ferry))
  (:objects a b c1)
  (:goal (and (at c1 b)))
  (:length (:serial 3) (:parallel 3))
  (:domain ferry))