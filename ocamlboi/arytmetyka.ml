type zakres = {low:float; upp:float} (* low - dolne ograniczenie, upp - górne ograniczenie *)

type wartosc = zakres list

let wartosc_dokladnosc x p = (* float -> float -> wartosc *)
    let a = (1. -. p/.100.)*.x
    and b = (1. +. p/.100.)*.x in{
    low = min a b;
    upp = max a b}::[]


let wartosc_od_do x y = { (* float -> float -> wartosc *)
    low = x;
    upp = y}::[]

let wartosc_dokladna x = {
    low = x; upp = x}::[]

let in_wartosc x y = (* wartosc -> float -> bool*)
    let rec pom l =
        match l with
        | [] -> false
        | h::t -> if h.low <= y && y <= h.upp then true
                    else pom t
    in pom x 

let min_pom x = (* wartosc -> float, zwraca dolne ograniczenie wartości x, może zwrócić nan *)
    let rec pom l min_w =
        match l with
        | [] -> min_w
        | h::t -> pom t (min min_w h.low)
    in pom x infinity

let max_pom x = (* wartosc -> float, zwraca górne ograniczenie wartości x, może zwrócić nan *)
    let rec pom l max_w =
        match l with
        | [] -> max_w
        | h::t -> pom t (max max_w h.upp)
    in pom x neg_infinity

let min_wartosc x = (* wartosc -> bool, korzysta z funkcji pomocniczych, by obsłużyć dzielenie przez 0 *)
    let a = min_pom x 
    and b = max_pom x in
    if (classify_float a) == FP_nan || (classify_float b) == FP_nan then
        nan 
    else a

let max_wartosc x = (* wartosc -> bool, korzysta z funkcji pomocniczych, by obsłużyć dzielenie przez 0 *)
    let a = min_pom x 
    and b = max_pom x in
    if (classify_float a) == FP_nan || (classify_float b) == FP_nan then
        nan 
    else b

let sr_wartosc x = (* wartosc -> bool *)
    let a = min_wartosc x
    and b = max_wartosc x in
    if a = neg_infinity && b = infinity then
        nan
    else (a+.b)/.2.


(* POMOCNICZE FUNKCJE SŁUŻĄCE DO SCALANIA ZAKRESÓW, W WARTOSCIACH MAJĄCYCH WSPÓLNĄ CZĘŚĆ *)

let overlaps_zakres a b = (* zakres -> zakres -> bool, czy zakresy a,b mają część wspólną *)
    if (classify_float a.low) == FP_nan || (classify_float a.upp) == FP_nan || (classify_float b.low) == FP_nan || (classify_float b.upp) == FP_nan then
        false
    else if not(a.upp < b.low || a.low > b.upp) then
        true
    else false

let merge_zakres a b = { (* zakres -> zakres -> zakres, łączy zakresy a,b *)
    low = min a.low b.low;
    upp = max a.upp b.upp
    }

let includes_zakres a b = (* zakres -> zakres -> bool, czy zakres b zawiera się w zakresie a *)
    if a.low<=b.low && a.upp>=b.upp then
        true
    else false

let rec includes_wartosc a b = (* zakres -> wartosc -> bool, czy zakres b zawiera się w wartosc a *)
    match a with
    | [] -> false
    | h::t -> if includes_zakres h b then
                    true
              else includes_wartosc t b

let merge_wartosc l = (* wartosc -> wartosc, scala wszystkie zakresy w wartosc l, które mają wspólną część*)
    let rec merge_pom_pom x l =
    match l with
    | [] -> x
    | h::t -> if overlaps_zakres x h then
                    let y = merge_zakres x h in
                    merge_pom_pom y t
              else merge_pom_pom x t
    in let rec merge_pom x l =
    match x with
    | [] -> l
    | h::t -> let y = merge_pom_pom h t in
            if includes_wartosc l y then
                merge_pom t l
            else merge_pom t (y::l)
    in merge_pom l []

(* KONIEC TYCH POMOCNICZYCH FUNKCJI *)

let plus_zakres a b = { (* zakres -> zakres -> zakres, dodaje 2 zakresy *)
    low = a.low +. b.low;
    upp = a.upp +. b.upp
}

let plus a b = (* wartosc -> wartosc -> wartosc, dodaje dwie wartości *)
    let rec pom_b a_z b_w l_w = 
        match b_w with
        | [] -> l_w
        | h::t -> let c = plus_zakres a_z h in
                pom_b a_z t (c::l_w)
    in let rec pom_a a_w l_w = 
        match a_w with
        | [] -> l_w
        | h::t -> let lista = pom_b h b l_w in
                pom_a t lista
    in let wynik = pom_a a []
    in merge_wartosc wynik

let minus_zakres a b = {    (* zakres -> zakres -> zakres, odejmuje zakres b od a *)
    low = a.low -. b.upp;
    upp = a.upp -. b.low
}

let minus a b = (* wartosc -> wartosc -> wartosc, odejmuje wartość b od a *)
    let rec pom_b a_z b_w l_w = 
        match b_w with
        | [] -> l_w
        | h::t -> let c = minus_zakres a_z h in
                pom_b a_z t (c::l_w)
    in let rec pom_a a_w l_w = 
        match a_w with
        | [] -> l_w
        | h::t -> let lista = pom_b h b l_w in
                pom_a t lista
    in let wynik = pom_a a []
    in merge_wartosc wynik

let razy_f a b =
    if (a = 0. && b = infinity) || (a = infinity && b = 0.) || (a = 0. && b = neg_infinity) || (a = neg_infinity && b = 0.) then
        0.
    else a *. b

let razy_zakres a b = { (* zakres -> zakres -> zakres, mnoży 2 zakresy *)
    low = min (min (min (razy_f a.low b.low) (razy_f a.low b.upp)) (razy_f a.upp b.low)) (razy_f a.upp b.upp);
    upp = max (max (max (razy_f a.low b.low) (razy_f a.low b.upp)) (razy_f a.upp b.low)) (razy_f a.upp b.upp)
}
let razy a b = (* wartosc -> wartosc -> wartosc, mnoży dwie wartości *)
    let rec pom_b a_z b_w l_w = 
        match b_w with
        | [] -> l_w
        | h::t -> let c = razy_zakres a_z h in
                pom_b a_z t (c::l_w)
    in let rec pom_a a_w l_w = 
        match a_w with
        | [] -> l_w
        | h::t -> let lista = pom_b h b l_w in
                pom_a t lista
    in let wynik = pom_a a []
    in merge_wartosc wynik


let rec podzielic_zakres a b = (* zakres -> zakres -> wartosc, dzieli zakres a przez b, zwraca wartosc bo wynikiem mogą być dwa przedziały *)
    if (b.low = 0. && b.upp = 0.) then  (* b = (0;0) *)
        [{low = nan; upp = nan}]
    else if b.upp > 0. && b.low < 0. then (* b = (c;d), c<0, d>0 *)
        let poma = podzielic_zakres a {low = b.low; upp = 0.}
        and pomb = podzielic_zakres a {low = 0.; upp = b.upp}
        in poma@pomb
    else if b.low = 0. then             (* b = (0;x) *)
        [{low = 
            if a.low < 0. then
                neg_infinity
            else 
                a.low/.b.upp;
        upp = 
            if a.upp <= 0. then
                a.upp/.b.upp
            else
                infinity}]
    else if b.upp = 0. then         (* b = (x,0) *)
        [{low = 
            if a.upp > 0. then
                neg_infinity
            else
                a.upp/.b.low;
        upp = 
            if a.low < 0. then
                infinity
            else
                a.low/.b.low}]
    else if b.upp < 0. then (* b = (c;d), c<d<0 *)
        [{low = 
            if a.upp < 0. then
                a.upp/.b.low
            else
                a.upp/.b.upp;
        upp = 
            if a.low > 0. then
                a.low/.b.low
            else
                a.low/.b.upp}]
    else                    (* b = (c,d) , 0<c<d *)
        [{low = 
            if a.low > 0. then
                a.low/.b.upp
            else
                a.low/.b.low;
        upp =
            if a.upp < 0. then
                a.upp/.b.upp
            else
                a.upp/.b.low}]

let podzielic a b =     (* wartosc -> wartosc -> wartosc, dzieli wartość a przez b *)
    let rec pom_b a_z b_w l_w = 
        match b_w with
        | [] -> l_w
        | h::t -> let c = podzielic_zakres a_z h in
                pom_b a_z t (c@l_w)
    in let rec pom_a a_w l_w = 
        match a_w with
        | [] -> l_w
        | h::t -> let lista = pom_b h b l_w in
                pom_a t lista
    in let wynik = pom_a a []
    in merge_wartosc wynik
