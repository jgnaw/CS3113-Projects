#include "Map.h"

Map::Map(
    int           mapColumns,
    int           mapRows,
    unsigned int *levelData,
    const char   *textureFilePath,
    float         tileSize,
    int           textureColumns,
    int           textureRows,
    Vector2       origin
)
    : mMapColumns{ mapColumns }, mMapRows{ mapRows }, mLevelData{ levelData },
      mTextureAtlas{ LoadTexture(textureFilePath) }, mTileSize{ tileSize },
      mTextureColumns{ textureColumns }, mTextureRows{ textureRows },
      mOrigin{ origin }
{
    build();
}

Map::~Map()
{
    UnloadTexture(mTextureAtlas);
}

void Map::build()
{
    // TODO: Compute the map's boundaries in world coordinates, then
    //       precompute the texture area (Rectangle) of every tile in the atlas
    //       and store them in mTextureAreas.
    mLeftBoundary = mOrigin.x - (mMapColumns * mTileSize) / 2.0f;
    mRightBoundary = mOrigin.x + (mMapColumns * mTileSize) / 2.0f;
    mTopBoundary = mOrigin.y - (mMapRows * mTileSize) / 2.0f;
    mBottomBoundary = mOrigin.y + (mMapRows * mTileSize) / 2.0f;

    for (int row = 0; row < mTextureRows; row++){
        for (int col = 0; col < mTextureColumns; col ++){
            Rectangle textureArea = {
                (float)col * ((float)mTextureAtlas.width / mTextureColumns),
                (float)row * ((float)mTextureAtlas.height / mTextureRows),

                (float)mTextureAtlas.width / mTextureColumns,
                (float)mTextureAtlas.height / mTextureRows
            };
            mTextureAreas.push_back(textureArea);
        }
    }
}

void Map::render()
{
    // TODO: Draw every non-empty tile of the map. A tile index of 0 means
    //       "draw nothing".
    for (int row = 0; row < mMapRows; row++){
        for (int col = 0; col < mMapColumns; col++){
            int tile = mLevelData[row * mMapColumns + col];

            if (tile == 0) continue;

            Rectangle destinationArea = {
                mLeftBoundary + col * mTileSize,
                mTopBoundary + row * mTileSize,

                mTileSize,
                mTileSize
            };

            DrawTexturePro(
                mTextureAtlas,
                mTextureAreas[tile - 1],
                destinationArea,
                {0.0f, 0.0f},
                0.0f,
                WHITE
            );
        }
    }
}

bool Map::isSolidTileAt(Vector2 position, float *xOverlap, float *yOverlap)
{
    // TODO: Return true if the tile at `position` is solid, and write how far
    //       `position` has penetrated into that tile to xOverlap and yOverlap.
    *xOverlap = 0.0f;
    *yOverlap = 0.0f;

    //in the map?
    if (position.x < mLeftBoundary || position.x > mRightBoundary ||
        position.y < mTopBoundary || position.y > mBottomBoundary) 
        return false;

    int tileXIndex = floor((position.x - mLeftBoundary) / mTileSize);
    int tileYIndex = floor((position.y - mTopBoundary) / mTileSize);
    if (tileXIndex < 0 || tileXIndex >= mMapColumns ||
        tileYIndex < 0 || tileYIndex >= mMapRows)
        return false;

    int tile = mLevelData[tileYIndex * mMapColumns + tileXIndex];
    if (tile == 0) return false;

    float tileCentreX = mLeftBoundary + tileXIndex * mTileSize + mTileSize / 2.0f;
    float tileCentreY = mTopBoundary + tileYIndex * mTileSize + mTileSize / 2.0f;

    *xOverlap = (mTileSize / 2.0f) - fabs(position.x - tileCentreX);
    *yOverlap = (mTileSize / 2.0f) - fabs(position.y - tileCentreY);
    return true;
}
