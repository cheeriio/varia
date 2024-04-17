{-# LANGUAGE OverloadedStrings #-}

import System.IO
import qualified Data.Text as T
import qualified Data.Char as Char

data Tile = Wall | Ground | Storage | Box | Blank deriving (Enum, Eq)

data Direction = R | U | L | D deriving (Enum, Eq)

data Coord = C Int Int
instance Eq Coord where C x_1 y_1 == C x_2 y_2 = x_1 == x_2 && y_1 == y_2
instance Show Coord where
  show (C x y) = "(" ++ show x ++ ", " ++ show y ++ ")"

isWalkable :: Tile -> Bool
isWalkable Ground = True
isWalkable Storage = True
isWalkable _ = False

wall, storage, ground, box :: String
box = "\ESC[93;49m$\ESC[39;49m"
ground = " "
storage = "\ESC[35;49m.\ESC[39;49m"
wall = "\ESC[31;49m#\ESC[39;49m"

data Maze = Maze Coord (Coord -> Tile)

maze_first = Maze (C 0 1) (\(C x y) -> case () of
  _ | abs x > 4  || abs y > 4  -> Blank
    | abs x == 4 || abs y == 4 -> Wall
    | x ==  2 && y <= 0        -> Wall
    | x ==  3 && y <= 0        -> Storage
    | x >= -2 && y == 0        -> Box
    | otherwise                -> Ground
    )
    
maze_3d_glasses = Maze (C 0 0) (\(C x y) -> case () of
    -- Horizontal patches
  _ | y == 7  && x >= 8 && x <= 11  -> Wall
    | y == 6 && x >= -13 && x <= 8 -> Wall
    | y == 4 && x >= -10 && x <= -1 -> Wall
    | y == 4 && x >= 1 && x <= 9 -> Wall
    | y == 3 && x >= -1 && x <= 1 -> Wall
    | y == -1 && x >= 9 && x <= 12 -> Wall
    | y == -2 && x >= -12 && x <= -9 -> Wall
    | y == -3 && x >= -1 && x <= 1 -> Wall
    | y == -4 && x >= -9 && x <= -1 -> Wall
    | y == -4 && x >= 1 && x <= 9 -> Wall
    -- Vertical patches
    | x == -13 && y >= 3 && y <= 6 -> Wall
    | x == -12 && y >= -2 && y <= 3 -> Wall
    | x == -10 && y >= 1 && y <= 4 -> Wall
    | x == -9 && y >= 1 && y <= 4 -> Wall
    | x == -9 && y >= 1 && y <= 4 -> Wall
    | x == -9 && y >= -4 && y <= -1 -> Wall
    | x == -1 && y >= -1 && y <= 4 -> Wall
    | x == 1 && y >= -4 && y <= 1 -> Wall
    | x == 9 && y >= -4 && y <= -1 -> Wall
    | x == 9 && y >= 1 && y <= 4 -> Wall
    | x == 11 && y >= 2 && y <= 7 -> Wall
    | x == 12 && y >= -1 && y <= 2 -> Wall
    
    | x >= -8 && x <= -2 && y >= -2 && y <= 2 && mod (x+y) 2 == 0 -> Box
    | x >= 2 && x <= 8 && y >= -2 && y <= 2 && mod (x+y) 2 == 0 -> Storage
    | abs x < 14 && y >= -5 && y <= 8 -> Ground
    | otherwise -> Blank
    )

maze_sokoban = Maze (C 5 0) (\(C x y) -> case () of
    -- Walls
  _ | y == 7  && x >= -9 && x <= 9  -> Wall
    | y == -7  && x >= -9 && x <= 9  -> Wall
    | x == -9 && y >= -7 && y <= 7 -> Wall
    | x == 9 && y >= -7 && y <= 7 -> Wall
    -- S
    | y == 3  && x >= -7 && x <= -5  -> Wall
    | elem (C x y) [(C (-7) 1), C (-6) 1, C(-5) 2,
                     C(-7) 4, C(-6) 5, C(-5) 5] -> Wall
    
    -- O
    | x == -3 && y >= 2 && y <= 5 -> Wall
    | x == -1 && y >= 2 && y <= 5 -> Wall
    | elem (C x y) [C (-2) 6, C(-2) 1] -> Wall
    
    -- K
    | x == 1 && y >= 1 && y <= 5 -> Wall
    | elem (C x y) [C 2 3, C 3 1, C 3 2, C 3 4, C 3 5] -> Wall
    
    -- O
    | x == 5 && y >= 2 && y <= 5 -> Wall
    | x == 7 && y >= 2 && y <= 5 -> Wall
    | elem (C x y) [C 6 6, C 6 1] -> Wall
    
    -- B
    | x == -7 && y >= -5 && y <= -1 -> Wall
    | elem (C x y) [C (-6) (-5), C (-6) (-3), C (-6) (-1),
                    C (-5) (-4), C (-5) (-2)] -> Wall
    
    -- A
    | x == -3 && y >= -5 && y <= -2 -> Wall
    | x == 0 && y >= -5 && y <= -2 -> Wall
    | y == -4  && x >= -3 && x <= 0 -> Wall
    | elem (C x y) [C (-2) (-1), C (-1) (-1)] -> Wall
    
    -- N
    | x == 2 && y >= -5 && y <= 0 -> Wall
    | x == 5 && y >= -5 && y <= -1 -> Wall
    | elem (C x y) [C 3 (-2), C 4 (-3)] -> Wall

    | x == 7 && y == -4 -> Wall
    | x == 7 && y == -1 -> Box
    | x == 7 && y == -5 -> Storage
    | abs x < 9 && abs y < 7 -> Ground
    | otherwise -> Blank
    )
    
maze_warmup = Maze (C 1 0) (\(C x y) -> case () of
    -- Horizontal patches
  _ | y == 4  && x >= -4 && x <= 0  -> Wall
    | y == -4  && x >= 0 && x <= 4  -> Wall
    | y == 1  && x >= 2 && x <= 4  -> Wall
    | y == -1  && x >= -2 && x <= 0 -> Wall

    -- Vertical patches
    | x == -4 && y >= 0 && y <= 4 -> Wall
    | x == 4 && y >= -4 && y <= 0 -> Wall
    | x == 0 && y >= -4 && y <= -2 -> Wall
    -- Single blocks
    | elem (C x y) [(C (-3) 0), (C 2 (-1)), (C 1 2), (C 0 3)] -> Wall
    
    | elem (C x y) [(C 0 0), (C 2 0), (C (-1) 1), (C (-2) 2)] -> Box
    | elem (C x y) [(C 1 1), (C (-1) 0), (C 0 2), (C (-1) 3)] -> Storage
    | abs x < 6 && abs y < 6 -> Ground
    | otherwise -> Blank
    )
    
maze_cliffhanger = Maze (C 0 0) (\(C x y) -> case () of
    -- Horizontal patches
  _ | y == 7  && x >= -7 && x <= -4  -> Wall
    | y == -8  && x >= 4 && x <= 7  -> Wall

    -- Vertical patches
    | x == -7 && y >= 3 && y <= 7 -> Wall
    | x == 7 && y >= -8 && y <= -4 -> Wall
    
    | x >= -6 && x <= 3 && y == -x - 3 -> Wall
    | x >= -3 && x <= 6 && y == -x + 3 -> Wall
    | x == 3 && y == -7 -> Wall
    
    | x >= -5 && x <= 3 && y == -x - 2 -> Storage
    | x >= -3 && x <= 5 && y == -x + 2 -> Storage
    | x == -5 && y == 5 -> Storage
    | x == 5 && y == -5 -> Storage
    
    | x >= -5 && x <= 4 && y == -x - 1 -> Box
    | x >= -4 && x <= 5 && y == -x + 1 -> Box

    | abs x < 9 && y >= -9 && y <= 8 -> Ground
    | otherwise -> Blank
    )


mazes :: [Maze]
mazes = [maze_first, maze_warmup, maze_cliffhanger, maze_3d_glasses]
badMazes :: [Maze]
badMazes = [maze_sokoban]


foldList :: (a -> b -> b) -> b -> [a] -> b
foldList _ b [] = b
foldList f b (h:t) = f h (foldList f b t)
-- No tail recursion; however, order is maintained

elemList :: Eq a => a -> [a] -> Bool
elemList e l = 
  foldList (\new_e ans -> e == new_e || ans) False l

appendList :: [a] -> [a] -> [a]
appendList a b = foldList (:) b a

listLength :: [a] -> Integer
listLength l = foldList (\_ ans -> ans + 1) 0 l

filterList :: (a -> Bool) -> [a] -> [a]
filterList f l = foldList (\e ans -> if (f e) then (e:ans) else ans) [] l

nth :: [a] -> Integer -> a
nth (h:t) n = snd (foldList
    (\e (cnt, val) -> if cnt == 1 then (0, e) else (cnt - 1, val))
    ((listLength (h:t)) - n + 1, h)
    (h:t))

mapList :: (a -> b) -> [a] -> [b]
mapList f l = foldList (\e ans -> ((f e):ans)) [] l

andList :: [Bool] -> Bool
andList l = foldList (\e ans -> e && ans) True l

allList :: (a-> Bool) -> [a] -> Bool
allList f l = andList (mapList f l)

isGraphClosed :: Eq a => a -> (a -> [a]) -> (a -> Bool) -> Bool

helper_dfs cur neighbours visited isOk =
  if not (isOk cur) then
    (cur:visited, False)
  else
    let to_visit = neighbours cur in
    if listLength to_visit == 0 then
      (cur:visited, True)
    else
      foldList
        (\v (new_visited, ans) ->
          if not ans then
            (new_visited, False)
          else if elemList v new_visited then
            (new_visited, True)
          else
            helper_dfs v neighbours (new_visited) isOk)
        (cur:visited, True)
        to_visit


isGraphClosed initial neighbours isOk = 
  snd (helper_dfs initial neighbours [] isOk)

reachable :: Eq a => a -> a -> (a -> [a]) -> Bool
reachable v initial neighbours =
  let (visited, _) = helper_dfs initial neighbours [] (\_ -> True) in
  elemList v visited

allReachable :: Eq a => [a] -> a -> (a -> [a]) -> Bool
allReachable vs initial neighbours =
  allList (\v -> reachable v initial neighbours) vs

isClosed :: Maze -> Bool
isClosed (Maze c maze_def) =
  (maze_def c == Ground || maze_def c == Storage) &&
  isGraphClosed
    c
    (\(C x y) ->
      if isWalkable (maze_def (C x y)) then
        [C (x+1) y, C (x-1) y, C x (y+1), C x (y-1)]
      else
        [])
    (\c -> (maze_def c) /= Blank)
    
isSane :: Maze -> Bool

isSane (Maze c maze_def) = 
  let (visited, _) = helper_dfs
        c
        (\(C x y) ->
        if isWalkable (maze_def (C x y)) then
          [C (x+1) y, C (x-1) y, C x (y+1), C x (y-1)]
        else
          [])
        []
        (\x -> (maze_def x) /= Blank) in
  let n_boxes =
        listLength (filterList (\x -> (maze_def x) == Box) visited) in
  let n_storages =
        listLength (filterList (\x -> (maze_def x) == Storage) visited) in
  n_boxes == n_storages

drawTile :: Tile -> String
drawTile Blank = " "
drawTile Wall = wall
drawTile Ground = ground
drawTile Storage = storage
drawTile Box = box

player :: Direction -> String
player _ = "@"

adjacentCoord :: Direction -> Coord -> Coord
adjacentCoord R (C x y) = C (x + 1) y
adjacentCoord L (C x y) = C (x - 1) y
adjacentCoord U (C x y) = C x (y + 1)
adjacentCoord D (C x y) = C x (y - 1)
  
moveCoords :: [Direction] -> Coord -> Coord
moveCoords [] c = c
moveCoords (d:ds) c = moveCoords ds (adjacentCoord d c)

data State = S {
  stPlayer :: Coord,
  stDir    :: Direction,
  boxPos   :: [Coord],
  level    :: Int,
  curMaze  :: Maze,
  mazesLeft:: [Maze],
  moves    :: Int
}
instance Eq State where 
  S c_1 d_1 b_1 l_1 _ _ m_1 == S c_2 d_2 b_2 l_2 _ _ m_2 =
    c_1 == c_2 && d_1 == d_2 && b_1 == b_2 && l_1 == l_2 && m_1 == m_2


initialBoxes :: Maze -> [Coord]
initialBoxes (Maze _ maze_def) =
  foldList 
    (\x acc -> foldList 
      (\y acc ->
        if maze_def (C x y) == Box then
          (C x y):acc
        else acc)
      acc
      [-20..20::Int])
    []
    [-20..20::Int]


removeBoxes :: (Coord -> Tile) -> Coord -> Tile
removeBoxes maze_def c
  | maze_def c == Box = Ground
  | otherwise = maze_def c

addBoxes :: [Coord] -> (Coord -> Tile) -> Coord -> Tile
addBoxes boxes maze_def c
  | elem c boxes = Box
  | otherwise = maze_def c

moveBox :: [Coord] -> Coord -> Direction -> [Coord]
moveBox [] _ _ = []
moveBox (c_1:boxes) c_2 d
  | c_1 == c_2 = (adjacentCoord d c_2):boxes
  | otherwise = c_1:(moveBox boxes c_2 d)


walkTo :: State -> Direction -> State
walkTo (S c _ boxes num (Maze start maze_def) levels m) d_2 =
 let new_coord = adjacentCoord d_2 c in
 if isWalkable ((addBoxes boxes (removeBoxes maze_def)) new_coord) then
   S new_coord d_2 boxes num (Maze start maze_def) levels (m+1)
 else if 
    ((addBoxes boxes (removeBoxes maze_def)) (adjacentCoord d_2 c)) == Box &&
     (isWalkable ((addBoxes boxes (removeBoxes maze_def)) (moveCoords [d_2, d_2] c))) then
   S (adjacentCoord d_2 c) d_2 (moveBox boxes (adjacentCoord d_2 c) d_2)
     num (Maze start maze_def) levels (m+1)
 else
   S c d_2 boxes num (Maze start maze_def) levels m

isWinning :: State -> Bool
isWinning (S _ _ boxes _ (Maze _ maze_def) _ _) =
  foldl (&&) True (map (\c -> maze_def c == Storage) boxes)

data Activity world = Activity
    world
    (Event -> world -> world)
    (world -> Screen)
    
data Event = KeyPress String
type Screen = String

type DrawFun = Coord -> String
type Picture = DrawFun -> DrawFun
blank = id
(&) = (.)


drawLevel :: (Coord -> Tile) -> DrawFun
drawLevel maze_def = (\c -> case () of
  _ | maze_def c == Wall -> wall
    | maze_def c == Box -> box
    | maze_def c == Storage -> storage
    | maze_def c == Ground -> ground
    | maze_def c == Blank -> ground
    )

addPlayer :: Coord -> Direction -> Picture
addPlayer c_p d draw c
  | c_p == c = player d
  | otherwise = draw c

handleEvent :: Event -> State -> State
handleEvent (KeyPress key) s
    | isWinning s || key == "N" =
      let (S c _ _ num _ levels _) = s in
      if listLength levels == 0 then
        s
      else
        let (next:t) = levels in
        let (Maze start maze_def) = next in
        S start D (initialBoxes next) (num+1) next t 0
    | key == "D" || key == "d" = walkTo s R
    | key == "W" || key == "w" = walkTo s U
    | key == "A" || key == "a" = walkTo s L
    | key == "S" || key == "s" = walkTo s D
    | otherwise = s

line_of_spaces :: Int -> String
line_of_spaces n = foldList (\e acc -> " " ++ acc) "" [1..n::Int]

empty_lines :: Int -> String
empty_lines n =
  let empty_line = line_of_spaces 80 in
  foldList (\e acc -> empty_line ++ "\n" ++ acc) "" [1..n::Int]

winning_screen :: Int -> Screen
winning_screen moves =
  let win_line = (line_of_spaces 32) ++ 
                 "Level finished" ++
                 (line_of_spaces 32) ++
                 "\n" in
  let moves_string = "Moves: " ++ show moves in
  let len = length moves_string in
  let moves_line = line_of_spaces ((80 - len) `div` 2) ++
                   moves_string ++
                   line_of_spaces (80 - len - ((80 - len) `div` 2)) ++
                   "\n" in
  empty_lines 7 ++ win_line ++ empty_lines 7 ++ moves_line ++ empty_lines 7

drawState :: State -> Screen
drawState s = 
  let (S c d boxes _ maze _ moves) = s in
  if isWinning s then
    winning_screen moves
  else
    let Maze _ maze_def = maze in
    foldList 
      (\y result_xy -> result_xy ++ (foldList 
        (\x result_x -> 
          ((addPlayer c d (drawLevel (addBoxes boxes (removeBoxes maze_def))))
            (C (x - 20) (y - 12))) ++ " " ++ result_x)
        ""
        [1..40::Int]) ++ "\n")
      ""
      [1..23::Int]

pictureOfBools :: [Bool] -> Screen
pictureOfBools xs =
  let title_line = (line_of_spaces 36) ++ "SOKOBAN" ++ (line_of_spaces 37) ++ "\n" in
  let (level_info, length) = 
                            foldList 
                              (\e (line, length) -> 
                                if e then 
                                  (" *" ++ line, length + 2)
                                else (" X" ++ line, length + 2))
                                  ("", 0) xs in
  let level_info_line = line_of_spaces ((80 - length) `div` 2) ++ 
                        level_info ++ 
                        line_of_spaces (80 - length - ((80 - length) `div` 2)) in
  empty_lines 7 ++ title_line ++ empty_lines 7 ++
    level_info_line ++ empty_lines 7

        
etap4 :: Screen
etap4 = pictureOfBools
  (map (\x -> (isSane x) && (isClosed x)) (appendList mazes badMazes))

startScreen :: Screen
startScreen = etap4

data SSState world = StartScreen | Running world deriving(Eq)

resettable :: Activity s -> Activity s
resettable (Activity state0 handle draw)
  = Activity state0 handle' draw
  where handle' (KeyPress key) _ | key == "Esc" = state0
        handle' e s = handle e s

withStartScreen :: Activity s -> Activity (SSState s)
withStartScreen (Activity state0 handle draw)
  = Activity state0' handle' draw'
  where
    state0' = StartScreen

    handle' (KeyPress key) StartScreen
         | key == " "                  = Running state0
    handle' _              StartScreen = StartScreen
    handle' e              (Running s) = Running (handle e s)

    draw' StartScreen = startScreen
    draw' (Running s) = draw s

data WithUndo a = WithUndo a [a]

withUndo :: Eq a => Activity a -> Activity (WithUndo a)
withUndo (Activity state0 handle draw) = Activity state0' handle' draw' where
    state0' = WithUndo state0 []
    handle' (KeyPress key) (WithUndo s stack) | key == "U" || key == "u"
      = case stack of s':stack' -> WithUndo s' stack'
                      []        -> WithUndo s []
    handle' e              (WithUndo s stack)
       | s' == s = WithUndo s stack
       | otherwise = WithUndo (handle e s) (s:stack)
      where s' = handle e s
    draw' (WithUndo s _) = draw s

runActivity :: Activity s -> IO ()
runActivity (Activity initial_state handler drawer) =
  let go =  (\state -> do
              let output_str = drawer state
              putStr "\ESCc"
              putStr output_str
              input <- getChar
              char_available <- hReady stdin
              let c = if input == '\ESC' && char_available then do 
                        c <- getChar
                        if c == '[' then do
                          c <- getChar
                          if c == 'D' then -- left arrow
                            return "A"
                          else if c == 'C' then -- right arrow
                            return "D"
                          else if c == 'A' then -- up arrow
                            return "W"
                          else if c == 'B' then -- down arrow 
                            return "S"
                          else
                            return " "
                        else
                          return [c]
                      else if input == '\ESC' then
                        return "Esc"
                      else
                        return [input]
              move <- c
              let new_state = handler (KeyPress (move)) state
              go new_state
            ) in do
  hSetBuffering stdin NoBuffering
  go initial_state

initialState :: State
initialState =
  let (h:t) = mazes in
  let (Maze c _) = h in
  S c D (initialBoxes h) 0 h t 0

etap5 :: IO ()
etap5 = runActivity (resettable(withUndo(withStartScreen(
          Activity initialState handleEvent drawState))))

main :: IO()
main = etap5