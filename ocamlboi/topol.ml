open PMap

exception Cykliczne

let topol lst =
    match lst with
    | [] -> []
    | h::t ->
    let dodane = ref empty  (* wierzchołki dodane do listy wynikowej *)
    and odwiedzone = ref empty  (* wierzchołki odwiedzone w aktualnej ścieżce *)
    and graf = ref empty
    and ans = ref [] in
    let merge l1 l2 =   (* połączenie dwóch list *)
        let temp = ref empty in
        let ans = ref [] in
        let f e =  
            begin
            temp := add e e !temp;
            ans := e::(!ans);
            end
        in let g e = 
            begin
            if mem e !temp then
                ()
            else
                begin
                temp := add e e !temp;
                ans := e::(!ans);
                end
            end
        in begin
        List.iter f l1;
        List.iter g l2;
        !ans;
        end
    in let rec buduj_graf l =
        match l with
        | [] -> ()
        | (e, elist)::t ->
            begin
            if mem e !graf then (* wierzchołek e był wcześniej dodany do grafu *)
                begin
                let prev = find e !graf in
                graf := remove e !graf;
                graf := add e (merge prev elist) !graf;
                buduj_graf t;
                end
            else    (* pierwszy raz dodajemy wierzchołek e *)
                begin
                graf := add e elist !graf;
                buduj_graf t;
                end;
            ()
            end
    in let rec dfs e =
        if mem e !dodane then
            ()
        else if mem e !odwiedzone then
            raise Cykliczne
        else
            let cel = if mem e !graf then find e !graf else []
            and f x = dfs x in
            begin
            odwiedzone := add e e !odwiedzone;
            List.iter f cel;
            dodane := add e e !dodane;
            ans := e::(!ans);
            ()
            end
    (* dodajemy do grafu wierzchołki niewyróżnione jawnie w liście, ale są do nich ścieżki *)
    in let uzupelnij ls =
        let f (e, elist) = 
            let g v =
                if not (mem v !graf) then
                    graf := add v [] !graf
            in List.iter g elist
        in List.iter f ls
    in let f (e, elist) = dfs e in
    begin
    buduj_graf lst;
    uzupelnij lst;
    List.iter f lst;
    !ans;
    end
