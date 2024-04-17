type 'a queue = Leaf | Node of 'a queue * 'a * int * 'a queue (* lewe poddrzewo * wartosc * prawa wysokosc * prawe poddrzewo *)

let empty = Leaf	(* 'a queue *)

let is_empty x = 	(* 'a queue -> bool, Zwraca [true] jeśli dana kolejka jest pusta. W przeciwnym razie [false] *)
	match x with
	| Leaf -> true
	| _ -> false

exception Empty 	(* Wyjątek podnoszony przez [delete_min] gdy kolejka jest pusta *)

let rec join a b = 		(* 'a queue -> 'a queue -> 'a queue, [join q1 q2] zwraca złączenie kolejek [q1] i [q2] *)
	match (a,b) with
	| (Leaf,p) -> p
	| (l,Leaf) -> l
	| (Node(l1,x1,_,r1),Node(_,x2,_,_)) ->
			if x1<=x2 then
				let q = join r1 b in
				match (q,l1) with
				| (Node(_,_,h3,_),Node(_,_,h1,_)) ->
				if h1 > h3 then
					Node(l1,x1,h3+1,q)
				else
					Node(q,x1,h1+1,l1)
				| _,_-> Node(q,x1,1,l1)
			else
				join b a

let add e q = (* 'a -> 'a pqueue -> 'a queue, [add e q] zwraca kolejkę powstałą z dołączenia elementu [e] do kolejki [q] *)
	join q (Node(Leaf,e,1,Leaf))

let delete_min q =	(* 'a queue -> 'a * 'a queue,  *)
	match q with							(* Dla niepustej kolejki [q], [delete_min q] zwraca parę [(e,q')] gdzie [e] *)
	| Leaf -> raise Empty					(* jest elementem minimalnym kolejki [q] a [q'] to [q] bez elementu [e]. *)
	| Node(l,x,_,r) -> (x,join l r)			(* Jeśli [q] jest puste podnosi wyjątek [Empty]. *)
