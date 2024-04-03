LoadPackage("grape");

grapher := function(E)
    local G;
    G := Graph(Group(()), [1..VertexCount(E)], OnPoints, function(x,y) return [x,y] in E or [y,x] in E; end);
    return G;
end;
Deg := function(X, v)
    local i, x;
    x := 0;
    for i in X do
        if v in i then
            x := x+1;
        fi;
    od;
    return x;
end;
Mood := function(X,E, v)
    local x,i,j;
    x := 0;
    for i in X do
        if v in i then
            x := x-1;
        fi;
    od;
    for j in Difference(E, X) do
        if v in j then
            x := x+1;
        fi;
    od;
    return x;
end;
Friends := function(X,E,v)
    local x, i;
    x := [];
    for i in Difference(E,X) do
        if i[1] = v then
            Add(x, i[2]);
        elif i[2] = v then
            Add(x, i[1]);
        fi;
    od;
    return x;
end;
VertexCount := function(E)
    local i, AllVertices;
    AllVertices := [];
    for i in E do
        Add(AllVertices, i[1]);
        Add(AllVertices, i[2]);
    od;
    return Length(DuplicateFreeList(AllVertices));
end;
UpperObjects := function(X, E, cube)
    local Uppers, i, Cands, Stab, S, A, Orbs, o;
    Uppers := [];
    S := grapher(E);
    A := AutomorphismGroup(S);
    Stab := Stabilizer(A, X, OnSetsSets);
    Orbs := Orbits(Stab, Eligibles(X,E, cube), OnSets);
    for o in Orbs do
    Add(Uppers, Union(X, [o[1]]));
    od;
    return Uppers;
end;
scan_step := function(X, E, G, cube)
    local C, Uppers, upper, fprimeimage, y, c, orbs, reps, o, negs,u,s,v,sigs,graphcovers,iso_free,r;
    C := [];
    Uppers := UpperObjects(X, E, cube);
    if not Uppers = [] then
        for upper in Uppers do
            if not upper = [] then
                Add(C, upper);
            fi;
        od;
        return C;
    else
        return [];
    fi;
end;
scan := function(X, E, G, cube)
    local counter, i, A, C, c, s, remain, graphcovers, iso_free,negs,u,v,r;
    remain := true;
    counter := 1;
    C:=[];
    C[1] := [X];
    while remain do
        C[counter + 1] := [];
        remain := false;
        for c in C[counter] do
            s := scan_step(c, E, G, cube);
            if not s = [] then
                remain := true;
                Add(C[counter + 1], s);
            fi;
        od;
        C[counter + 1]:=Concatenation(C[counter + 1]);
        orbs := Orbits(G, AsSet(C[counter+1]), OnSetsSets);
        reps := [];
        for o in orbs do
            Add(reps, o[1]);
        od;
        graphcovers := [];
        for c in reps do
            Add(graphcovers, Covers(E, VertexCount(E), c));
        od;
        iso_free := GraphIsomorphismClassRepresentatives(graphcovers);
        C[counter+1] := [];
        for l in iso_free do
            negs := [];
            for edge in UndirectedEdges(l) do
                if edge[1]>VertexCount(E) and edge[2] <= VertexCount(E) then
                    Add(negs, AsSet([edge[1]-VertexCount(E), edge[2]]));
                elif edge[2]>VertexCount(E) and edge[1] <= VertexCount(E) then
                    Add(negs, AsSet([edge[2]-VertexCount(E), edge[1]]));
                fi;
            od;
            Add(C[counter+1], DuplicateFreeList(negs));
        od;
        counter := counter + 1;
    od;
    A:=[];
    for i in [1..counter] do
        A:=Union(A, C[i]);
    od;
    return A;
end;
Covers := function(e, v, m)
    local Negs, i, j, k, AllEdges, PosEdges, Strands, Edges;
    Negs := [];
    for i in m do
        Add(Negs, i);
        Add(Negs, [i[1] + v, i[2] + v]);
    od;
    AllEdges := [];
    for j in e do
        Add(AllEdges, j);
        Add(AllEdges, [j[1]+v, j[2] +v]);
    od;
    PosEdges := Difference(AllEdges, Negs);
    Strands := [];
    for k in m do
        Add(Strands, [k[1], k[2] + v]);
        Add(Strands, [k[1]+v, k[2]]);
    od;
    Edges := Concatenation(PosEdges, Strands);
    return Graph(Group(()), [1..2*v], OnPoints, function(x,y) return [x,y] in Edges or [y,x] in Edges; end);
end;
SIClasses := function(X, E, cube)
    local x, i, S, A,s;
    S := grapher(E);
    A := AutomorphismGroup(S);
    x := [];
    s := scan(X, E, A, cube);
    for i in s do
        Add(x, Covers(E, VertexCount(E), i));
    od;
    return Length(GraphIsomorphismClassRepresentatives(x));
end;
SIClassReps := function(X, E, cube)
    local x, i, S, A;
    S := grapher(E);
    A := AutomorphismGroup(S);
    x := [];
    for i in scan(X, E, A, cube) do
        Add(x, Covers(E, VertexCount(E), i));
    od;
    return GraphIsomorphismClassRepresentatives(x);
end;
Eligibles := function(X,E, cube)
    local Candidates, Rejects, V;
    if cube = 1 then
    Candidates:=Difference(E,X);
    Rejects:=[];
    V := DuplicateFreeList(Union(X));
    for i in Candidates do
    if not Intersection(V, i) = [] then
    Add(Rejects, i);
    fi;
    od;
    return Difference(Candidates, Rejects);
    else
    Candidates := [];
    Rejects := [];
    for i in Difference(E,X) do
    if Deg(E,i[1]) - Deg(X, i[1]) > Deg(X, i[1]) + 1 and Deg(E, i[2]) - Deg(X, i[2]) > Deg(X, i[2]) + 1 then
    Add(Candidates, i);
    fi;
    od;
    for i in Candidates do
    if not i in Rejects then
    for c in Difference(Friends(X,E,i[1]), [i[2]]) do
    for d in Difference(Friends(X,E,c), [i[1], i[2]]) do
    for x in Difference(Friends(X,E,i[1]), [i[2], c, d]) do
    y := AsSet([x, d]);
    if y in Difference(E,X) then
    if AsSet([c,x]) in Difference(E,X) then
    if AsSet([d,i[1]]) in Difference(E,X)     then
    if Mood(X,E,i[1])+Mood(X,E,x)+ Mood(X,E,c)+ Mood(X,E,d) - 14 < 0     then
    Add(Rejects, i);
    fi;
    elif not AsSet([d,i[1]]) in E then
    if Mood(X,E,i[1])+Mood(X,E,x)+Mood(X,E,c)+Mood(X,E,d) - 12 < 0     then
    Add(Rejects, i);
    fi;
    fi;
    elif not AsSet([c,x]) in E then
    if AsSet([d,i[1]]) in Difference(E,X)     then
    if Mood(X,E,i[1])+Mood(X,E,x)+Mood(X,E,c)+Mood(X,E,d) - 12 < 0     then
    Add(Rejects, i);
    fi;
    elif not AsSet([d,i[1]]) in E then
    if Mood(X,E,i[1])+Mood(X,E,x)+Mood(X,E,c)+Mood(X,E,d) - 10 < 0     then
    Add(Rejects, i);
    fi;
    fi;
    fi;
    elif not y in E then
    if AsSet([c,x]) in Difference(E,X) then
    if AsSet([d,i[1]]) in Difference(E,X)     then
    if Mood(X,E,i[1])+Mood(X,E,x)+Mood(X,E,c)+Mood(X,E,d) - 12 < 0     then
    Add(Rejects, i);
    fi;
    elif not AsSet([d,i[1]]) in E then
    if Mood(X,E,i[1])+Mood(X,E,x)+Mood(X,E,c)+Mood(X,E,d) - 10 < 0     then
    Add(Rejects, i);
    fi;
    fi;
    elif not AsSet([c,x]) in E then
    if AsSet([d,i[1]]) in Difference(E,X)     then
    if Mood(X,E,i[1])+Mood(X,E,x)+Mood(X,E,c)+Mood(X,E,d) - 10 < 0     then
    Add(Rejects, i);
    fi;
    elif not AsSet([d,i[1]]) in E then
    if Mood(X,E,i[1])+Mood(X,E,x)+Mood(X,E,c)+Mood(X,E,d) - 8 < 0     then
    Add(Rejects, i);
    fi;
    fi;
    fi;
    elif y in X then
    if AsSet([c,x]) in Difference(E,X) then
    if AsSet([d,i[1]]) in Difference(E,X)     then
    if Mood(X,E,i[1])+Mood(X,E,x)+Mood(X,E,c)+Mood(X,E,d) - 10 < 0     then
    Add(Rejects, i);
    fi;
    elif not AsSet([d,i[1]]) in E then
    if Mood(X,E,i[1])+Mood(X,E,x)+Mood(X,E,c)+Mood(X,E,d) - 8 < 0     then
    Add(Rejects, i);
    fi;
    fi;
    elif not AsSet([c,x]) in E then
    if AsSet([d,i[1]]) in Difference(E,X)     then
    if Mood(X,E,i[1])+Mood(X,E,x)+Mood(X,E,c)+Mood(X,E,d) - 8 < 0     then
    Add(Rejects, i);
    fi;
    elif not AsSet([d,i[1]]) in E then
    if Mood(X,E,i[1])+Mood(X,E,x)+Mood(X,E,c)+Mood(X,E,d) - 6 < 0     then
    Add(Rejects, i);
    fi;
    fi;
    fi;
    fi;
    od;
    od;
    od;
    fi;
    if not i in Rejects then
    for c in Difference(Friends(X,E,i[2]), [i[1]]) do
    for d in      Difference(Friends(X,E,c), [i[1], i[2]]) do
    for x in      Difference(Friends(X,E,i[2]), [i[1], c, d]) do
    y := AsSet([x, d]);
    if y in Difference(E,X) then
    if AsSet([c,x]) in Difference(E,X) then
    if AsSet([d,i[2]]) in Difference(E,X)     then
    if Mood(X,E,i[2])+Mood(X,E,x)+Mood(X,E,c)+Mood(X,E,d) - 14 < 0     then
    Add(Rejects, i);
    fi;
    elif not AsSet([d,i[2]]) in E then
    if Mood(X,E,i[2])+Mood(X,E,x)+Mood(X,E,c)+Mood(X,E,d) - 12 < 0     then
    Add(Rejects, i);
    fi;
    fi;
    elif not AsSet([c,x]) in E then
    if AsSet([d,i[2]]) in Difference(E,X)     then
    if Mood(X,E,i[2])+Mood(X,E,x)+Mood(X,E,c)+Mood(X,E,d) - 12 < 0     then
    Add(Rejects, i);
    fi;
    elif not AsSet([d,i[2]]) in E then
    if Mood(X,E,i[2])+Mood(X,E,x)+Mood(X,E,c)+Mood(X,E,d) - 10 < 0     then
    Add(Rejects, i);
    fi;
    fi;
    fi;
    elif not y in E then
    if AsSet([c,x]) in Difference(E,X) then
    if AsSet([d,i[2]]) in Difference(E,X)     then
    if Mood(X,E,i[2])+Mood(X,E,x)+Mood(X,E,c)+Mood(X,E,d) - 12 < 0     then
    Add(Rejects, i);
    fi;
    elif not AsSet([d,i[2]]) in E then
    if Mood(X,E,i[2])+Mood(X,E,x)+Mood(X,E,c)+Mood(X,E,d) - 10 < 0     then
    Add(Rejects, i);
    fi;
    fi;
    elif not AsSet([c,x]) in E then
    if AsSet([d,i[2]]) in Difference(E,X)     then
    if Mood(X,E,i[2])+Mood(X,E,x)+Mood(X,E,c)+Mood(X,E,d) - 10 < 0     then
    Add(Rejects, i);
    fi;
    elif not AsSet([d,i[2]]) in E then
    if Mood(X,E,i[2])+Mood(X,E,x)+Mood(X,E,c)+Mood(X,E,d) - 8 < 0     then
    Add(Rejects, i);
    fi;
    fi;
    fi;
    elif y in X then
    if AsSet([c,x]) in Difference(E,X) then
    if AsSet([d,i[2]]) in Difference(E,X)     then
    if Mood(X,E,i[2])+Mood(X,E,x)+Mood(X,E,c)+Mood(X,E,d) - 10 < 0     then
    Add(Rejects, i);
    fi;
    elif not AsSet([d,i[2]]) in E then
    if Mood(X,E,i[2])+Mood(X,E,x)+Mood(X,E,c)+Mood(X,E,d) - 8 < 0     then
    Add(Rejects, i);
    fi;
    fi;
    elif not AsSet([c,x]) in E then
    if AsSet([d,i[2]]) in Difference(E,X)     then
    if Mood(X,E,i[2])+Mood(X,E,x)+Mood(X,E,c)+Mood(X,E,d) - 8 < 0     then
    Add(Rejects, i);
    fi;
    elif not AsSet([d,i[2]]) in E then
    if Mood(X,E,i[2])+Mood(X,E,x)+Mood(X,E,c)+Mood(X,E,d) - 6 < 0     then
    Add(Rejects, i);
    fi;
    fi;
    fi;
    fi;
    od;
    od;
    od;
    fi;
    od;
    fi;
    return Difference(Candidates, Rejects);
end;
PrintSignedGraph := function(E, SE)
    n := VertexCount(E);
    Print(n, " ", Length(E), "\n");
    for e in E do
        Print(e[1], ",", e[2], ",");
        if e in SE then
            Print("1, ");
        else
            Print("0, ");
        fi;
    od;
    Print("\n");
end;
Groups := function(X,E, cube)
    local groups, covers, P, n, i, k, K, a, A, S, F;
    groups:=[];
    covers := SIClassReps(X,E, cube);
    P := [];
    n := VertexCount(E);
    for i in [1..n] do
        Add(P, [i, i+n]);
    od;
    k := MappingPermListList([1..n],[n+1..2*n]);
    K := Group(k);
    for a in covers do
        A := AutomorphismGroup(a);
        S := Stabilizer(A, P, OnSetsSets);
        F:=FactorGroup(S,K);
        Add(groups, F);
    od;
    return List(groups, StructureDescription);
end;
Signings := function(X,E,cube)
    local S, sigs, n, strands, edges, x, s;
    S := SIClassReps(X,E,cube);
    sigs := [];
    n := VertexCount(E);
    for s in S do
        strands:=[];
        edges := UndirectedEdges(s);
        for x in edges do
            if x[1] <= n and x[2] > n then
                Add(strands, AsSet([x[1], x[2]-n]));
            fi;
        od;
        Add(sigs, DuplicateFreeList(strands));
    od;
    return sigs;
end;
PrintIsoSignatures := function(X, E, cube)
    sigs := Signings(X, E, cube);
    for sig in sigs do
        PrintSignedGraph(E, sig);
    od;
end;
