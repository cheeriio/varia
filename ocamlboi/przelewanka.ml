let przelewanka input =
  let n = Array.length input in
  let volume = Array.map fst input in
  let final = Array.map snd input in
  let checked = Hashtbl.create 100 in (* zapamiętuje sprawdzone stany *)
  let q = Queue.create () in (* kolejka stanów które trzeba przeanalizować *)
  let ans = ref (-1) in
  let ifend = ref false in
  let rec gcd a b =
    if a = 0 then b
    else gcd (b mod a) a in
  (* sprawdza czy cur jest stanem koncowym, generuje kolejne stany powstale z cur *)
  let nextstep (cur, t) =
    if not (Hashtbl.mem checked cur) then
      begin
        Hashtbl.add checked cur 0;
        (* test czy stan to wynik *)
        ifend := true;
        for i = 0 to n - 1 do
          if not (cur.(i) = final.(i)) then
            ifend := false;
        done;
        if !ifend then
          ans := t;
        
        let cpy = Array.copy cur in
        for i = 0 to n - 1 do
          if not (cur.(i) = volume.(i)) then (* wypelnij wodą *)
            begin
              cur.(i) <- volume.(i);
              Queue.add (Array.copy cur, t + 1) q;
              cur.(i) <- cpy.(i);
            end;
          if not (cur.(i) = 0) then
            begin (* wylej wode *)
              cur.(i) <- 0;
              Queue.add (Array.copy cur, t + 1) q;
              cur.(i) <- cpy.(i);
              for j = 0 to n - 1 do (* przelej z i do j *)
                if not (i = j) && cur.(j) < volume.(j) then
                begin
                  cur.(j) <- cur.(j) + cur.(i);
                  cur.(i) <- 0;
                  if cur.(j) > volume.(j) then
                    begin
                      cur.(i) <- cur.(j) - volume.(j);
                      cur.(j) <- volume.(j);
                    end;
                  Queue.add (Array.copy cur, t + 1) q;
                  cur.(i) <- cpy.(i);
                  cur.(j) <- cpy.(j);
                end;
              done;
            end;
        done;
      end; in
  match input with
  | [||] -> 0
  | _ ->
  let possible = ref true in
  begin
    let div = Array.fold_left gcd 0 volume in (* nwd wszystkich końcowych wartosci *)
    if not (div = 0) then
      for i = 0 to n-1 do
        if not (final.(i) mod div = 0) then
          possible := false; (* nie da sie dojsc do stanu koncowego *)
      done;
    (* na końcu jedna szklanka musi być pusta lub pełna *)
    if not (Array.exists (fun (v,f) -> f = 0 || f = v) input) then
      possible := false;
    if !possible then
      begin
        Queue.add (Array.make n 0, 0) q;
        while not (Queue.is_empty q) && !ans = (-1) do
          nextstep (Queue.take q);
        done;
      end;
    !ans;
  end
