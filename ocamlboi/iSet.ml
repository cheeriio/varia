type inter = {low:int; upp:int}

type set =
  | Empty
  | Node of set * inter * set * int (* lewe poddrzewo * przedział * prawe poddrzewo * głębokość *)

type t =
  {
    cmp : inter -> inter -> int;
    set : set
  }

let height = function
  | Node (_, _, _, h) -> h
  | Empty -> 0

let make l k r = Node (l, k, r, max (height l) (height r) + 1)

let bal l k r =   (* tworzy zbalansowane drzewo, łącząc drzewa l, r i przedział k *)
  let hl = height l in
  let hr = height r in
  if hl > hr + 2 then
    match l with
    | Node (ll, lk, lr, _) ->
        if height ll >= height lr then make ll lk (make lr k r)
        else
          (match lr with
          | Node (lrl, lrk, lrr, _) ->
              make (make ll lk lrl) lrk (make lrr k r)
          | Empty -> assert false)
    | Empty -> assert false
  else if hr > hl + 2 then
    match r with
    | Node (rl, rk, rr, _) ->
        if height rr >= height rl then make (make l k rl) rk rr
        else
          (match rl with
          | Node (rll, rlk, rlr, _) ->
              make (make l k rll) rlk (make rlr rk rr)
          | Empty -> assert false)
    | Empty -> assert false
  else Node (l, k, r, max hl hr + 1)

let rec min_elt = function  (* zwraca minimalny przedział z drzewa *)
  | Node (Empty, k, _, _) -> k
  | Node (l, _, _, _) -> min_elt l
  | Empty -> raise Not_found

let rec max_elt = function  (* zwraca maksymalny przedział z drzewa *)
  | Node (_, k, Empty, _) -> k
  | Node (_, _, r, _) -> max_elt r
  | Empty -> raise Not_found

let rec remove_min_elt = function (* zwraca drzewo bez minimalnego przedziału *)
  | Node (Empty, _, r, _) -> r
  | Node (l, k, r, _) -> bal (remove_min_elt l) k r
  | Empty -> invalid_arg "PSet.remove_min_elt"

let rec remove_max_elt = function   (* zwraca drzewo bez maksymalnego przedziału *)
  | Node (l, _, Empty, _) -> l
  | Node (l, k, r, _) -> bal l k (remove_max_elt r)
  | Empty -> invalid_arg "PSet.remove_max_elt"

let merge t1 t2 = (* łączy drzewa t1 i t2 o różnych przedziałach *)
  match t1, t2 with
  | Empty, _ -> t2
  | _, Empty -> t1
  | _ ->
      let k = min_elt t2 in
      bal t1 k (remove_min_elt t2)

let icomp a b = (* 0 - a, b maja wspólną część albo sąsiadują ze sobą; < 0, > 0 - analogicznie do compare *)
  if a.low = min_int && a.upp = max_int then 0
  else if a.low = min_int then
    (if a.upp + 1 < b.low then (-1) else 0)
  else if a.upp = max_int then
    (if a.low - 1 > b.upp then 1 else 0)
  (* a - przedzial "normalnych" liczb *)
  else if (a.upp >= b.low && a.low <= b.low)||(b.upp >= a.low && b.low <= a.low || a.upp + 1 = b.low || a.low - 1 = b.upp) then 0
  else if a.upp < b.low then (-1) else 1 

let icontains a b = (* 0 - b należy do a, < 0, > 0 - analogicznie do compare *)
  if a.low <= b && a.upp >= b then 0
  else if b < a.low then (-1)
  else 1  (* b > a.upp *)

let imerge a b = (* lączy przedziały a, b *)
  {low = min a.low b.low; upp = max a.upp b.upp}

let create cmp = { cmp = cmp; set = Empty }

let empty = { cmp = icomp; set = Empty }  (* tworzy puste drzewo *)

let is_empty x = 
  x.set = Empty

let rec add_one cmp x = function  (* dodaje x do drzewa *)
  | Node (l, k, r, h) ->
      let c = cmp x k in
      if c = 0 then 
        let temp_k = imerge k x in
        let ifleft = if l = Empty then -1   (* czy w l jest część x *)
          else icomp (max_elt l) temp_k in
        let ifright = if r = Empty then -1    (* czy w r jest część x *)
          else icomp (min_elt r) temp_k in
        let nl = if ifleft = 0 then 
          add_one cmp (imerge (max_elt l) temp_k) (remove_max_elt l)
        else l in
        let nr = if ifright = 0 then
          add_one cmp (imerge (min_elt r) temp_k) (remove_min_elt r)
        else r in
        if ifleft = 0 && ifright = 0 then
          let temp = max_elt nl in
          bal (remove_max_elt nl) (imerge temp (min_elt nr)) (remove_min_elt nr)
        else if ifleft = 0 then
          bal (remove_max_elt nl) (max_elt nl) nr
        else if ifright = 0 then
          bal nl (min_elt nr) (remove_min_elt nr)
        else
          bal nl temp_k nr
      else if c < 0 then
        let nl = add_one cmp x l in
        bal nl k r
      else
        let nr = add_one cmp x r in
        bal l k nr
  | Empty -> Node (Empty, x, Empty, 1)

let add (x1,x2) { cmp = cmp; set = set } =
  { cmp = cmp; set = add_one cmp {low = x1; upp = x2} set }

let rec join cmp l v r =  (* pomocnicza *)
  match (l, r) with
    (Empty, _) -> add_one cmp v r
  | (_, Empty) -> add_one cmp v l
  | (Node(ll, lv, lr, lh), Node(rl, rv, rr, rh)) ->
      if lh > rh + 2 then bal ll lv (join cmp lr v r) else
      if rh > lh + 2 then bal (join cmp l v rl) rv rr else
      make l v r

let split x { cmp = cmp; set = set } =
  let rec loop x = function
      Empty ->
        (Empty, false, Empty)
    | Node (l, v, r, _) ->
        let c = icontains v x in
        if c = 0 then 
          if v.low < x && v.upp > x then
            ((add_one cmp {low = v.low; upp = x - 1} l), true, (add_one cmp {low = x + 1; upp = v.upp} r))
          else if v.low = x && v.upp = x then
            (l, true, r)
          else if v.low = x then
            (l, true, add_one cmp {low = v.low + 1; upp = v.upp} r)
          else (* v.upp = x *)
            (add_one cmp {low = v.low; upp = v.upp - 1} l , true, r)
        else if c < 0 then
          let (ll, pres, rl) = loop x l in (ll, pres, join cmp rl v r)
        else
          let (lr, pres, rr) = loop x r in (join cmp l v lr, pres, rr)
  in
  let setl, pres, setr = loop x set in
  { cmp = cmp; set = setl }, pres, { cmp = cmp; set = setr }

let ioverlaps a b = (* zwraca trójkę: ile przedzialow powstanie z (a - b) (0/1/2) * info czy lewe poddrzewo trzeba sprawdzić * info czy prawe; pomocnicza do remove*)
  if a.low > b.low && a.upp < b.upp then (0,true,true)

  else if a.low > b.low && a.upp = b.upp then (0,true,false)
  else if a.low > b.low && a.upp > b.upp then (1,true,false)

  else if a.low = b.low && a.upp < b.upp then (0,false,true)
  else if a.low < b.low && a.upp < b.upp then (1,false,true)

  else if a.low = b.low && a.upp > b.upp then (1,false,false)
  else if a.low < b.low && a.upp = b.upp then (1,false,false)

  else if a.low < b.low && a.upp > b.upp then (2,false,false)
  else (* a.low = b.low && a.upp = b.upp *) (0,false,false)

let iminus1 a b =
  if a.low < b.low then
    {low = a.low; upp = b.low -1}
  else {low = b.upp + 1; upp = a.upp}

let iminus2 a b = 
  ({low = a.low; upp = b.low - 1}, {low = b.upp + 1; upp = a.upp})

let remove (x_low, x_upp) { cmp = cmp; set = set } =  (* moze dziala *)
  let x = {low = x_low; upp = x_upp} in
  let rec loop = function
    | Node (l, k, r, _) ->
        let c = cmp x k in
        if c = 0 then 
          let (n,ifleft,ifright) = ioverlaps k x in
            if n = 0 then
              merge (if ifleft then loop l else l) (if ifright then loop r else r)
            else if n = 1 then
              bal (if ifleft then loop l else l) (iminus1 k x) (if ifright then loop r else r)
            else
              let (a, b) = iminus2 k x in
              bal (if ifleft then loop l else l) a (add_one cmp b (if ifright then loop r else r))
      else if c < 0 then bal (loop l) k r else bal l k (loop r)
    | Empty -> Empty in
  { cmp = cmp; set = loop set }

let mem x { cmp = cmp; set = set } =
  let rec loop = function
    | Node (l, k, r, _) ->
        let c = icontains k x in
        c = 0 || loop (if c < 0 then l else r)
    | Empty -> false in
  loop set

let exists = mem

let iter f { set = set } =
  let rec loop = function
    | Empty -> ()
    | Node (l, k, r, _) -> loop l; f (k.low, k.upp); loop r in
  loop set

let fold f { cmp = cmp; set = set } acc =
  let rec loop acc = function
    | Empty -> acc
    | Node (l, k, r, _) ->
          loop (f (k.low, k.upp) (loop acc l)) r in
  loop acc set

let elements { set = set } = 
  let rec loop acc = function
      Empty -> acc
    | Node(l, k, r, _) -> loop ((k.low,k.upp) :: (loop acc r)) l in
  loop [] set

let below x { set = set } =
  let rec loop acc = function
    | Empty -> acc
    | Node(l, {low = a1; upp = a2}, r, _) ->
        let acc_l = loop acc l in
        if acc_l < 0 || acc_l = max_int then max_int else
        if x > a2 then
          if a2/2 - a1/2 - 1 >= max_int/2 then
            max_int
          else loop (acc_l + a2 - a1 + 1) r
        else if a1 <= x && x <= a2 then
          if x/2 - a1/2 - 1 >= max_int/2 then
            max_int
          else acc_l + x - a1 + 1
        else (* x < a1 *)
          acc_l
in let w = loop 0 set in
if w < 0 then max_int else w
