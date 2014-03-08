#include "console/consoleTypes.h"
#include "debug/profiler.h"
#include "sim/simObject.h"
#include "2d/sceneobject/CompositeSprite.h"
#include "TileGrid.h"
//Dijkstra
//#include <boost/config.hpp>
//#include <boost/graph/graph_traits.hpp>
//#include <boost/graph/adjacency_list.hpp>
//#include <boost/graph/dijkstra_shortest_paths.hpp>

//A*
#include <boost/heap/priority_queue.hpp>
#include <boost/graph/astar_search.hpp>


#include <boost/graph/random.hpp>
#include <boost/random.hpp>
#include <boost/graph/graphviz.hpp>
//Not including time
#include <vector>
#include <list>
#include <iostream>
#include <fstream>
#include <math.h>    // for sqrt

IMPLEMENT_CONOBJECT(TileGrid);  

#define ACTOR_NEAR_TILE_EDGE_PERCENT 0.25
#define SQRT2 (F32)1.414214
#define SQRT5 (F32)2.236068

using namespace boost;
using namespace std;
//typedef F32 cost;

// euclidean distance heuristic
/*template <class Graph, class CostType, class LocMap>
class distance_heuristic : public astar_heuristic<Graph, CostType> {
public:
  typedef typename graph_traits<Graph>::vertex_descriptor Vertex;
  distance_heuristic(LocMap l, Vertex goal) : m_location(l), m_goal(goal) {}
  CostType operator()(Vertex u) {
    CostType dx = m_location[m_goal].x - m_location[u].x;
    CostType dy = m_location[m_goal].y - m_location[u].y;
    return ::sqrt(dx * dx + dy * dy);
  }
private:
  LocMap m_location;
  Vertex m_goal;
};*/

bool TileGrid::tryAStar(Tile* origin, Tile* goal) {
	//TestFindIntersection();
	std::priority_queue<Tile*, vector<Tile*>, TileDistanceCompare> openSet;
	Tile* current;
	Tile* neighbor;
	U32 baseX, baseY;
	F32 currentCostPast;
	for(U32 i = 0; i <= maxIndex(); i++) { 
		mTiles[i].mInClosedSet = false;
		mTiles[i].mCostPast = 99999;
		mTiles[i].mCameFrom = 0;
	}
	openSet.push(origin);
	origin->mCostPast = 0;
	origin->mCostFuture = origin->estimatedDistance(goal); // F32 mCostFuture; -> fScore
	while(!openSet.empty()) {
		current = openSet.top();
		openSet.pop();
		if(current->mInClosedSet) continue;

		if(current == goal) return true;

		current->mInClosedSet = true;
		currentCostPast = current->mCostPast;
		baseX = current->mLogicalX;
		baseY = current->mLogicalY;
		
		if(baseX > 0) { // Left
			if(baseY > 0) if(TryToAddNeighbor(current, -1, -1, currentCostPast, goal, neighbor, SQRT2)) openSet.push(neighbor);
			if(TryToAddNeighbor(current, -1, 0, currentCostPast, goal, neighbor, 1)) openSet.push(neighbor);
			if(baseY < mSizeY) if(TryToAddNeighbor(current, -1, 1, currentCostPast, goal, neighbor, SQRT2)) openSet.push(neighbor);

			// Knight's Moves - Left Secondary
			//if(baseY > 1) if(TryToAddNeighbor(current, -1, -2, currentCostPast, goal, neighbor, SQRT5)) openSet.push(neighbor);
			//if(baseY + 1 < mSizeY && (baseX - 1) * (baseY + 2) <= maxIndex()) if(TryToAddNeighbor(current, -1, 2, currentCostPast, goal, neighbor, SQRT5)) openSet.push(neighbor);

			if(baseX > 1) { // Knight's Moves - Left Primary
				//if(baseY > 0) if(TryToAddNeighbor(current, -2, -1, currentCostPast, goal, neighbor, SQRT5)) openSet.push(neighbor);
				//if(baseY < mSizeY) if(TryToAddNeighbor(current, -2, 1, currentCostPast, goal, neighbor, SQRT5)) openSet.push(neighbor);
			}
		}
		// Up/down
		if(baseY > 0) if(TryToAddNeighbor(current, 0, -1, currentCostPast, goal, neighbor, 1)) openSet.push(neighbor);
		if(baseY < mSizeY && (baseX * (baseY + 1)) <= maxIndex())  if(TryToAddNeighbor(current, 0, 1, currentCostPast, goal, neighbor, 1)) openSet.push(neighbor);
		if(baseX < mSizeX) { // Right
			if(baseY > 0) if(TryToAddNeighbor(current, 1, -1, currentCostPast, goal, neighbor, SQRT2)) openSet.push(neighbor);
			if((baseX+1) * baseY <= maxIndex()) if(TryToAddNeighbor(current, 1, 0, currentCostPast, goal, neighbor, 1)) openSet.push(neighbor);
			if(baseY < mSizeY && (baseX + 1) * (baseY + 1) <= maxIndex()) if(TryToAddNeighbor(current, 1, 1, currentCostPast, goal, neighbor, SQRT2)) openSet.push(neighbor);

			// Knight's Moves - Right Secondary
			//if(baseY > 1) if(TryToAddNeighbor(current, 1, -2, currentCostPast, goal, neighbor, SQRT5)) openSet.push(neighbor);
			//if(baseY + 1 < mSizeY && (baseX + 1) * (baseY + 2) <= maxIndex()) if(TryToAddNeighbor(current, 1, 2, currentCostPast, goal, neighbor, SQRT5)) openSet.push(neighbor);

			if(baseX + 1 < mSizeX) { // Knight's Moves - Right Primary
				//if(baseY > 0 && (baseX + 2) * (baseY - 1) <= maxIndex()) if(TryToAddNeighbor(current, 2, -1, currentCostPast, goal, neighbor, SQRT5)) openSet.push(neighbor);
				//if(baseY < mSizeY && (baseX + 2) * (baseY + 1) <= maxIndex()) if(TryToAddNeighbor(current, 2, 1, currentCostPast, goal, neighbor, SQRT5)) openSet.push(neighbor);
			}
		}
	}
	return false;
	//std::priority_queue<Tile*, 
	//std::priority_queue<Tile*, 
	/*
	astar_search_no_init
    (const VertexListGraph &g,
     typename graph_traits<VertexListGraph>::vertex_descriptor s,
     AStarHeuristic h, const bgl_named_params<P, T, R>& params);
	 */
	//typedef boost::heap::priority_queue<PathFindingNode *, boost::heap::compare<MyClassCompare> > MyPriorityQueue;
	
	//MyQueue* q = new MyQueue
	//typedef boost::heap::prioririty_queue<Tile *, boost::heap::compare<TileGrid> > queue;
}

void TileGrid::copyTo(SimObject* object) {  
    // Fetch other object.  
   TileGrid* pTileGrid = static_cast<TileGrid*>( object );  
  
   // Sanity!  
   AssertFatal(pTileGrid != NULL, "TileGrid::copyTo() - Object is not the correct type.");  
  
   // Copy parent.  
   Parent::copyTo( object );  
  
   // Copy the state.  

   //pTileGrid->mEmitLight = mEmitLight;  
   //pTileGrid->mBrightness = mBrightness;  
} 

bool TileGrid::onAdd()  {  
    // Fail if the parent fails.  
    if (!Parent::onAdd())  
        return false;  
  
    // Do some work here.  
    Con::printf("Hello from TileGrid!");  
	return true;
}  
  
void TileGrid::onRemove() {  
	// Do some work here.  
} 

TileGrid::TileGrid() {
	mDisplayed = false;
	resizeGrid(5,5);
	setDisplayableSize(3,3);
	mStrideX = 1;
	mStrideY = 1;
	mSpriteOffsetX = 0;
	mSpriteOffsetY = 0;

	mDebugSavedX = 0;
	mDebugSavedY = 0;
}
TileGrid::TileGrid(const U32 x, const U32 y) {  
	mDisplayed = false;
	Con::printf("Hello from TileGrid! Initializing...");  
	setDisplayableSize(3,3);
	resizeGrid(x,y);
	//mTiles = new Tile[x*y];
	mSizeX = x;
	mSizeY = y;
	mStrideX = 1;
	mStrideY = 1;
	mSpriteOffsetX = 0;
	mSpriteOffsetY = 0;
}  

void TileGrid::resizeGrid(const U32 x, const U32 y) {
	bool isDisplayed = mDisplayed;
	if(isDisplayed) setDisplayed(false);

	mTiles = new Tile[x*y];
	mSizeX = x;
	mSizeY = y;
	
	setDisplayCenter((x/2),(y/2)); // Look at the center of the grid
	setDisplayed(isDisplayed);
}


TileGrid::~TileGrid() {
	delete mTiles;
}

void TileGrid::addWall(const U32 x, const U32 y, TileRelativePosition positionRelativeToTile, const char* assetID, const U32 frame, const char* logicalPositionArgs) {
	//void initializeWall(const char* assetID, const U32 frame, Point2D* center, const char* logicalPosition, Tile* tileOne, TileRelativePosition positionRelativeToTileOne, Tile* tileTwo);
	if(!isValidLocation(x,y)) return;
	Wall* newWall = new Wall();
	Point2D* center = new Point2D();
	F32 centerX, centerY;

	U32 secondX = x + (positionRelativeToTile == TILE_LEFT ? -1 : positionRelativeToTile == TILE_RIGHT ? +1 : 0);
	U32 secondY = y + (positionRelativeToTile == TILE_DOWN ? -1 : positionRelativeToTile == TILE_UP ? +1 : 0);

	//if(positionRelativeToTile == TILE_LEFT || positionRelativeToTile == TILE_UP) {
	getTileCenter(x, y, centerX, centerY);
	//} else {
	//	getTileCenter(secondX, secondY, centerX, centerY);
	//}
	center->x = centerX;
	center->y = centerY;
	Tile* tileOne = getTile(x, y);
	Tile* tileTwo = (isValidLocation(secondX, secondY) ? getTile(secondX, secondY) : 0);

	newWall->initializeWall(assetID, frame, center, logicalPositionArgs, tileOne, positionRelativeToTile, tileTwo, mForegroundSprite);
}

void TileGrid::setTile(const U32 x, const U32 y, const char* tileAssetID, const U32 frame, const char* logicalPositionArgs) {
	Vector2 center;
	getTileCenter(x, y, center.x, center.y);
	mTiles[index(x,y)].initializeTile(tileAssetID, frame, logicalPositionArgs, center, x, y);
}

void TileGrid::updateTile(const U32 x, const U32 y, const char* tileAssetID, const U32 frame) {
	if(x < 0 || y < 0 || x > mSizeX || y > mSizeY || (x*y) > maxIndex()) return;
	mTiles[index(x,y)].updateTile(tileAssetID, frame);
}

void TileGrid::spinTile(const U32 x, const U32 y) {
	//tryAStar();
	if(x < 0 || y < 0 || x > mSizeX || y > mSizeY || (x*y) > maxIndex()) return;
	mTiles[index(x,y)].spinTile();
}

void TileGrid::setDisplayableSize(const U32 x, const U32 y) {
	// Can truncate the remainder...
	mNumDisplayedTilesFromCenterX = (U32)(x / 2);
	mNumDisplayedTilesFromCenterY = (U32)(y / 2);
}

void TileGrid::setDisplayed(bool displayed) {
	if(displayed == mDisplayed) return;
	if(displayed) {
		for(U32 x = mMinDisplayedX; x <= mMaxDisplayedX; x++) addColumn(x, mMinDisplayedY, mMaxDisplayedY);
	} else {
		for(U32 x = mMinDisplayedX; x <= mMaxDisplayedX; x++) removeColumn(x, mMinDisplayedY, mMaxDisplayedY);
	}

	mDisplayed = displayed;
}



void TileGrid::setDisplayCenter(const U32 x, const U32 y) {
	if(!mDisplayed) {
		mCenterX = x;
		mCenterY = y;

		mMinDisplayedX = constrainU32ToRange(x, -1 * (S32)mNumDisplayedTilesFromCenterX, 0, mSizeX - 1);
		mMaxDisplayedX = constrainU32ToRange(x,  1 * (S32)mNumDisplayedTilesFromCenterX, 0, mSizeX - 1);

		mMinDisplayedY = constrainU32ToRange(x, -1 * (S32)mNumDisplayedTilesFromCenterY, 0, mSizeY - 1);
		mMaxDisplayedY = constrainU32ToRange(x,  1 * (S32)mNumDisplayedTilesFromCenterY, 0, mSizeY - 1);
	} else {
		if ((x==mCenterX) && (y==mCenterY)) return;

		U32 newMinX;
		U32 newMinY;
		U32 newMaxX;
		U32 newMaxY;

		newMinX = constrainU32ToRange(x, -1 * (S32)mNumDisplayedTilesFromCenterX, 0, mSizeX - 1);
		newMaxX = constrainU32ToRange(x,  1 * (S32)mNumDisplayedTilesFromCenterX, 0, mSizeX - 1);

		newMinY = constrainU32ToRange(y, -1 * (S32)mNumDisplayedTilesFromCenterY, 0, mSizeY - 1);
		newMaxY = constrainU32ToRange(y,  1 * (S32)mNumDisplayedTilesFromCenterY, 0, mSizeY - 1);

		if(newMinX < mMinDisplayedX) {
			for(U32 curColumn = newMinX; curColumn < (std::min(mMinDisplayedX,newMaxX)); curColumn++) addColumn(curColumn, newMinY, newMaxY); // Don't include mMinDisplayedX because it's already present
		} else if(newMinX > mMinDisplayedX) {
			for(U32 curColumn = mMinDisplayedX; curColumn < newMinX; curColumn++) removeColumn(curColumn, mMinDisplayedY, mMaxDisplayedY); // Don't include new minX because don't want to remove it
		}
		// Max value increased: add columns
		if(newMaxX > mMaxDisplayedX) {
			for(U32 curColumn = std::max(newMinX,mMaxDisplayedX+1); curColumn <= newMaxX; curColumn++) addColumn(curColumn, newMinY, newMaxY);
		} else if(newMaxX < mMaxDisplayedX) { // Max value decreased: remove columns
			for(U32 curColumn = newMaxX + 1; curColumn <= mMaxDisplayedX; curColumn++) removeColumn(curColumn, mMinDisplayedY, mMaxDisplayedY);
		}

		if(newMinY < mMinDisplayedY) {
			for(U32 curRow = newMinY; curRow < std::min(newMaxY+1,mMinDisplayedY); curRow++) addRow(curRow, newMinX, newMaxX); 
		} else if(newMinY > mMinDisplayedY) {
			for(U32 curRow = mMinDisplayedY; curRow < newMinY; curRow++) removeRow(curRow, mMinDisplayedX, mMaxDisplayedX);
		}

		// Max value increased: add rows
		if(newMaxY > mMaxDisplayedY) {
			for(U32 curRow = std::max(newMinY,mMaxDisplayedY+1); curRow <= newMaxY; curRow++) addRow(curRow, newMinX, newMaxX);
		} else if(newMaxY < mMaxDisplayedY) { // Max value decreased: remove rows
			for(U32 curRow = newMaxY + 1; curRow <= mMaxDisplayedY; curRow++) removeRow(curRow, mMinDisplayedX, mMaxDisplayedX);
		}
		
		mCenterX = x;
		mCenterY = y;

		mMinDisplayedX = newMinX;
		mMinDisplayedY = newMinY;
		mMaxDisplayedX = newMaxX;
		mMaxDisplayedY = newMaxY;
	}
}

void TileGrid::updateWindowCenter() {
		if(mSceneWindow) {
			F32 x;
			F32 y;
			U32 logicalX, logicalY;

			mSceneWindow->getWindowCenter(x,y);
			x = x + mSpriteOffsetX;
			y = y + mSpriteOffsetY;
			
			getLogicalCoordinates(x,y,logicalX,logicalY,true);
			setDisplayCenter(logicalX,logicalY);
		} else Con::printf("No scene window mapped!"); 
}

Tile* TileGrid::getTile(const U32 x, const U32 y)  {
	Tile* t = &mTiles[index(x,y)];
	return t;
}

bool TileGrid::getRelativeMove(const U32 fromX, const U32 fromY, const S32 offsetX, const S32 offsetY, U32& toX, U32& toY) {
	// Returns true if there is a move to be made
	//toX = fromX + offsetX;
	//toY = fromY + offsetY;

	toX = constrainU32ToRange(fromX, offsetX, 0, mSizeX - 1);
	toY = constrainU32ToRange(fromY, offsetY, 0, mSizeY - 1);

	/*
	if(toX >= mSizeX) toX = mSizeX - 1;
	else if(toX  < 0) toX = 0;

	if(toY >= mSizeY) toY = mSizeY - 1;
	else if(toY < 0) toY = 0;
	*/
	
	if(toX == fromX && toY == fromY) {
		return false;
	} else {
		return true;
	}
	
}

void TileGrid::tryDijkstras() {
	/*
	typedef adjacency_list < listS, vecS, directedS, no_property, property < edge_weight_t, int > > graph_t;
	typedef graph_traits < graph_t >::vertex_descriptor vertex_descriptor;
	typedef graph_traits < graph_t >::edge_descriptor edge_descriptor;
	typedef std::pair<int, int> Edge;

	const int num_nodes = 5;
  //names of graph nodes
  enum nodes { A, B, C, D, E };
  char name[] = "ABCDE";
  //edges of the graph
  Edge edge_array[] = { Edge(A, C), Edge(B, B), Edge(B, D), Edge(B, E),
    Edge(C, B), Edge(C, D), Edge(D, E), Edge(E, A), Edge(E, B)
  };
  //weights/travelling costs for the edges
  int weights[] = { 1, 2, 1, 2, 7, 3, 1, 1, 1 };
  int num_arcs = sizeof(edge_array) / sizeof(Edge);

  //graph created from the list of edges
  graph_t g(edge_array, edge_array + num_arcs, weights, num_nodes);
  //create the property_map from edges to weights
  property_map<graph_t, edge_weight_t>::type weightmap = get(edge_weight, g);

  //create vectors to store the predecessors (p) and the distances from the root (d)
  std::vector<vertex_descriptor> p(num_vertices(g));
  std::vector<int> d(num_vertices(g));
  //create a descriptor for the source node
  vertex_descriptor s = vertex(A, g);

  //evaluate dijkstra on graph g with source s, predecessor_map p and distance_map d
  //note that predecessor_map(..).distance_map(..) is a bgl_named_params<P, T, R>, so a named parameter
  dijkstra_shortest_paths(g, s, predecessor_map(&p[0]).distance_map(&d[0]));

	std::stringstream ss;
	ss << "Distances and parents: ";
	
	graph_traits <graph_t>::vertex_iterator vi, vend;
	for (tie(vi, vend) = vertices(g); vi != vend; ++vi) {
		ss << "| distance(" << name[*vi] << ") = " << d[*vi] << ", ";
		ss << "parent(" << name[*vi] << ") = " << name[p[*vi]] << " |";
	}
	Con::printf(ss.str().c_str());
	*/
}

bool TileGrid::getLogicalCoordinates(const F32 worldX, const F32 worldY, U32& logicalX, U32& logicalY, bool constrainWithinGrid) {
	// Get the number of strides in from each edge
	F32 worldXOverStrideX = (worldX / mStrideX);
	F32 worldYOverStrideY = (worldY / mStrideY);

	if(!constrainWithinGrid) { // Check whether we're out of bounds
		if(worldYOverStrideY > 0) {
			if(worldYOverStrideY > (worldXOverStrideX + 1.5)) return false;
		} else {
			if((-1 * worldYOverStrideY) > (worldXOverStrideX + 0.5)) return false;
		}

	}
	
	// Logical Y = the row. Add the X and Y because with isometric projection, if you draw a line from left to right your row is increasing
	if(worldXOverStrideX + worldYOverStrideY < 0) logicalY = 0;
	else logicalY = (U32)((worldXOverStrideX + worldYOverStrideY) / 2);
	// Logical X = the column. Subtract the row from column. The easiest way to visualize this is to think of the far left column - it goes up and right as row increases
	if(logicalY > worldXOverStrideX) {
		if(constrainWithinGrid) logicalX = 0;
		else return false;
	} else {
		logicalX = (U32)(((worldXOverStrideX)-logicalY));
	}
	
	// World center of the tile we've determined. We need to check if this is in the top-left, top-right, bottom-left, or bottom-right quadrants of the square that can be drawn around the tile
	F32 logicalXWorldCenter = 0;
	F32 logicalYWorldCenter = 0;

	getTileCenter(logicalX, logicalY, logicalXWorldCenter, logicalYWorldCenter);

			std::stringstream ss2;
			ss2 << "TileGri9d got tile center: " << logicalX << "," << logicalY << " for " << worldX << "," << worldY;
			//Con::printf(ss2.str().c_str());

	F32 dispXPercentFromCenter = (worldX - logicalXWorldCenter) / mStrideX;
	F32 dispYPercentFromCenter = (worldY - logicalYWorldCenter) / mStrideY;
	
	if(std::abs(dispXPercentFromCenter) + std::abs(dispYPercentFromCenter) > 1) {
		if(dispXPercentFromCenter > 0) {
			if(dispYPercentFromCenter > 0) logicalY++;
			else logicalX++;
		}
		else {
			if(dispYPercentFromCenter > 0) {
				if(logicalX > 0) {
					logicalX--;
				} else {
					if(constrainWithinGrid) logicalX = 0;
					else return false;
				}
			} else {
				if(logicalY > 0) {
					logicalY--;
				} else {
					if(constrainWithinGrid) logicalY = 0;
					else return false;
				}
			}
		}
	}

	if(constrainWithinGrid) {
		if(logicalX >= mSizeX) logicalX = mSizeX - 1;
		// else if(logicalX < 0) logicalX = 0; Won't ever happen - U32

		if(logicalY >= mSizeY) logicalY = mSizeY - 1;
		// else if(logicalY < 0) logicalY = 0; Won't ever happen - U32
					
		return true;
	}
	else {
		return(isValidLocation(logicalX, logicalY));
	}
}

void TileGrid::rotateCamera(CameraPosition newOrientation) {
	CameraPosition oldOrientation = mCurrentCameraRotation;
	switch(newOrientation) {
		case CAMERA_NORTH: 
			mBackgroundSprite->setSpriteFlipX = false;
			mBackgroundSprite->setSpriteFlipY = false;
			mForegroundSprite->setSpriteFlipX = false;
			mForegroundSprite->setSpriteFlipY = false;
			break;
		case CAMERA_EAST:
			mBackgroundSprite->setSpriteFlipX = false;
			mBackgroundSprite->setSpriteFlipY = true;
			mForegroundSprite->setSpriteFlipX = false;
			mForegroundSprite->setSpriteFlipY = true;
			break;
		case CAMERA_SOUTH:
			mBackgroundSprite->setSpriteFlipX = true;
			mBackgroundSprite->setSpriteFlipY = true;
			mForegroundSprite->setSpriteFlipX = true;
			mForegroundSprite->setSpriteFlipY = true;
			break;
		case CAMERA_WEST:
			mBackgroundSprite->setSpriteFlipX = true;
			mBackgroundSprite->setSpriteFlipY = false;
			mForegroundSprite->setSpriteFlipX = true;
			mForegroundSprite->setSpriteFlipY = false;
			break;
	}
}

void TileGrid::initPersistFields() {  
    // Call parent.  
    Parent::initPersistFields();  
	
	addField("SceneWindow",TypeSimObjectPtr, Offset(mSceneWindow, TileGrid), "Scene window viewing this tile grid.");
	addField("CompositeSprite",TypeSimObjectPtr, Offset(mBackgroundSprite, TileGrid), "Composite sprite to use for this tile grid.");
	addField("ForegroundSprite",TypeSimObjectPtr, Offset(mForegroundSprite, TileGrid), "Composite sprite to use for this tile grid.");
    // Add my fields here.  
   //addField("EmitLight", TypeBool, Offset(mEmitLight, TileGrid), "Flags whether the light is on or off.");  
    //addField("Brightness", TypeF32, Offset(mBrightness, TileGrid), "Sets the brightness of the light.");   
}                      