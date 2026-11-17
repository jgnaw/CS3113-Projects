#ifndef ENTITY_H
#define ENTITY_H

#include "Map.h"

enum Direction    { LEFT, UP, RIGHT, DOWN              }; // For walking
enum Action       { P_WALKING, P_IDLE, P_ATTACK, P_HURT, P_DEATH };
enum EntityStatus { ACTIVE, INACTIVE                   };
enum EntityType   { PLAYER, BLOCK, PLATFORM, NPC, PORTAL, NONE };
enum AIType       { WANDERER, FOLLOWER, CHASER         };
enum AIState      { WALKING, IDLE, LOCKED           };

class Entity
{
private:
    Vector2 mPosition;
    Vector2 mMovement;
    Vector2 mVelocity;
    Vector2 mAcceleration;

    Vector2 mScale;
    Vector2 mColliderDimensions;
    Vector2 mRenderOffset = {0.0f, 0.0f};

    Texture2D   mTexture;
    TextureType mTextureType;
    Vector2     mSpriteSheetDimensions;

    std::map<Action, std::vector<int>> mAnimationAtlas;
    std::vector<int>                      mAnimationIndices;
    Direction                             mDirection;
    Action                                mAction;
    int                                   mFrameSpeed;

    bool mHasHit = false;
    int   mCurrentFrameIndex = 0;
    float mAnimationTime     = 0.0f;

    bool  mIsJumping    = false;
    float mJumpingPower = 0.0f;

    int   mSpeed;
    float mAngle;

    bool mIsCollidingTop    = false;
    bool mIsCollidingBottom = false;
    bool mIsCollidingRight  = false;
    bool mIsCollidingLeft   = false;

    EntityStatus mEntityStatus = ACTIVE;
    EntityType   mEntityType;

    AIType  mAIType;
    AIState mAIState;

    bool isColliding(Entity *other) const;

    void checkCollisionY(Entity *collidableEntities, int collisionCheckCount);
    void checkCollisionY(Map *map);

    void checkCollisionX(Entity *collidableEntities, int collisionCheckCount);
    void checkCollisionX(Map *map);

    void resetColliderFlags()
    {
        mIsCollidingTop    = false;
        mIsCollidingBottom = false;
        mIsCollidingRight  = false;
        mIsCollidingLeft   = false;
    }

    void animate(float deltaTime);
    void AIActivate(Entity *target, Map *map);
    void AIWander(Map *map);
    void AIFollow(Entity *target);
    void AIChase(Entity *target, Map *map);

public:
    static constexpr int   DEFAULT_SIZE          = 250;
    static constexpr int   DEFAULT_SPEED         = 200;
    static constexpr int   DEFAULT_FRAME_SPEED   = 14;
    static constexpr float Y_COLLISION_THRESHOLD = 0.5f;

    Entity();
    Entity(
        Vector2     position,
        Vector2     scale,
        const char *textureFilepath,
        EntityType  entityType
    );
    Entity(
        Vector2                               position,
        Vector2                               scale,
        const char                           *textureFilepath,
        TextureType                           textureType,
        Vector2                               spriteSheetDimensions,
        std::map<Action, std::vector<int>> animationAtlas,
        EntityType                            entityType
    );
    ~Entity();

    void update(
        float   deltaTime,
        Entity *player,
        Map    *map,
        Entity *collidableEntities,
        int     collisionCheckCount
    );
    void render();
    void normaliseMovement() { Normalise(&mMovement); }

    void jump()       { mIsJumping = true;         }
    void activate()   { mEntityStatus = ACTIVE;    }
    void deactivate() { mEntityStatus = INACTIVE;  }
    void displayCollider();

    bool isActive() { return mEntityStatus == ACTIVE; }

    void moveUp()    { mMovement.y = -1; mDirection = UP;    }
    void moveDown()  { mMovement.y =  1; mDirection = DOWN;  }
    void moveLeft()  { mMovement.x = -1; mDirection = LEFT;  }
    void moveRight() { mMovement.x =  1; mDirection = RIGHT; }

    void resetMovement() { mMovement = { 0.0f, 0.0f }; }

    Vector2     getPosition()              const { return mPosition;              }
    Vector2     getMovement()              const { return mMovement;              }
    Vector2     getVelocity()              const { return mVelocity;              }
    Vector2     getAcceleration()          const { return mAcceleration;          }
    Vector2     getScale()                 const { return mScale;                 }
    Vector2     getColliderDimensions()    const { return mColliderDimensions;    }
    Vector2     getSpriteSheetDimensions() const { return mSpriteSheetDimensions; }
    Texture2D   getTexture()               const { return mTexture;               }
    TextureType getTextureType()           const { return mTextureType;           }
    Direction   getDirection()             const { return mDirection;             }
    int         getFrameSpeed()            const { return mFrameSpeed;            }
    float       getJumpingPower()          const { return mJumpingPower;          }
    bool        isJumping()                const { return mIsJumping;             }
    int         getSpeed()                 const { return mSpeed;                 }
    float       getAngle()                 const { return mAngle;                 }
    EntityType  getEntityType()            const { return mEntityType;            }
    AIType      getAIType()                const { return mAIType;                }
    AIState     getAIState()               const { return mAIState;               }
    Vector2     getRenderOffset()          const { return mRenderOffset;          }
    Action      getAction()                const { return mAction;                }

    bool isCollidingTop()    const { return mIsCollidingTop;    }
    bool isCollidingBottom() const { return mIsCollidingBottom; }

    std::map<Action, std::vector<int>> getAnimationAtlas() const { return mAnimationAtlas; }

    void setPosition(Vector2 newPosition)
        { mPosition = newPosition;                 }
    void setMovement(Vector2 newMovement)
        { mMovement = newMovement;                 }
    void setAcceleration(Vector2 newAcceleration)
        { mAcceleration = newAcceleration;         }
    void setScale(Vector2 newScale)
        { mScale = newScale;                       }
    void setTexture(const char *textureFilepath)
        { mTexture = LoadTexture(textureFilepath); }
    void setColliderDimensions(Vector2 newDimensions)
        { mColliderDimensions = newDimensions;     }
    void setSpriteSheetDimensions(Vector2 newDimensions)
        { mSpriteSheetDimensions = newDimensions;  }
    void setSpeed(int newSpeed)
        { mSpeed = newSpeed;                       }
    void setFrameSpeed(int newSpeed)
        { mFrameSpeed = newSpeed;                  }
    void setJumpingPower(float newJumpingPower)
        { mJumpingPower = newJumpingPower;         }
    void setAngle(float newAngle)
        { mAngle = newAngle;                       }
    void setEntityType(EntityType entityType)
        { mEntityType = entityType;                }
    void setAIState(AIState newState)
        { mAIState = newState;                     }
    void setAIType(AIType newType)
        { mAIType = newType;                       }
    void setDirection(Direction newDirection)
    {
        mDirection = newDirection;

        if (mTextureType == ATLAS)
            mAnimationIndices = mAnimationAtlas.at(mAction);
    }
    //shift where sprites are drawn to fit colliders
    void setRenderOffset(Vector2 newRenderOffset)
        { mRenderOffset = newRenderOffset; }
    void setAction(Action newAction)
        { mAction = newAction; }
    bool overlaps(Entity *other) {
        return isColliding(other);
    }
    //enable hurt animation
    void hurt(){
        mAction = P_HURT;
        mCurrentFrameIndex = 0;
        mAnimationTime = 0.0f;
        if (mTextureType == ATLAS)
            mAnimationIndices = mAnimationAtlas.at(mAction);
    }
    //enable attack
    void attack(){
        if (mAction != P_ATTACK && mAction != P_HURT) {
            mAction = P_ATTACK;
            mCurrentFrameIndex = 0;
            mAnimationTime = 0.0f;
            if (mTextureType == ATLAS)
                mAnimationIndices = mAnimationAtlas.at(mAction);
            mHasHit = false;
        }
    }
    //handle logic for attack hitbox
    bool attackLanded(float width, float height, float offset, int activeFrame1, int activeFrame2, Entity *other){
        if (!other->isActive()) return false;
        if (mHasHit) return false;
        float dir = mDirection == LEFT ? -1.0f : 1.0f;
        float xHitbox = mPosition.x + dir * (offset + width / 2.0f);
        float xDistance = fabs(xHitbox - other->getPosition().x) -
            ((width + other->getColliderDimensions().x) / 2.0f);
        float yDistance = fabs(mPosition.y - other->getPosition().y) -
            ((height + other->getColliderDimensions().y) / 2.0f);
        mHasHit = xDistance < 0.0f && yDistance < 0.0f &&
                 (mAnimationIndices[mCurrentFrameIndex] == activeFrame1 || 
                  mAnimationIndices[mCurrentFrameIndex] == activeFrame2);
        return mHasHit;
    }
    //determine if enemy attack in range of player 
    bool inRange(Entity *other, float xRange, float yRange) const {
        return fabs(mPosition.x - other->getPosition().x) < xRange &&
               fabs(mPosition.y - other->getPosition().y) < yRange;
    }
};

#endif // ENTITY_H
