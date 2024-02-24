cbuffer PerView : register(b0)
{
    float4x4 View;
    float4x4 Proj;
};

struct VertexInputType
{
    uint id : SV_VertexID;
    float3 position : POSITION;
    float4 color : COLOR;
    float2 uv : TEXCOORD;
    float lifetime : LIFETIME;
    float rotation : ROTATION;
    float2 size : SIZE;
};

struct PixelInputType
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
    float2 uv : TEXCOORD; // texture sample uv for the current frame
    float2 uv2 : TEXCOORD1; // texture sample uv for the next frame
    float4x4 viewMat : VIEW;
    float4 viewPosition : VIEW_POSITION;
    float4 particleCenter : PARTICLE_CENTER;
    float frameFraction : FRAME_FRACTION;
    float nearPlane : NEAR_PLANE; // near plane (bigger value because of reversed depth)
    float farPlane : FAR_PLANE; // far plane (smaller value because of reversed depth)
};

const static int tilesWidth = 8;
const static int tilesHeight = 8;
const static int totalTiles = tilesWidth * tilesHeight;

const static float tileUVWidth = 1.0f / tilesWidth;
const static float tileUVHeight = 1.0f / tilesHeight;

PixelInputType main(VertexInputType input)
{
    PixelInputType output;

    float3 dir = normalize(-input.position);

    float3 worldUp = float3(0, 1, 0);
    float3 right = cross(worldUp, dir);
    float3 up = cross(dir, right);
    

    // Determine the offset based on the vertex ID
    float2 offset;
    uint id = input.id % 4;

    if (id == 0)
    {
        offset = float2(-0.5, -0.5);
        input.uv = float2(0, 1);
    }
    else if (id == 1)
    {
        offset = float2(-0.5, 0.5);
        input.uv = float2(0, 0);
    }
    else if (id == 2)
    {
        offset = float2(0.5, 0.5);
        input.uv = float2(1, 0);
    }
    else if (id == 3)
    {
        offset = float2(0.5, -0.5);
        input.uv = float2(1, 1);
    }
    
    
    

    // Apply the rotation
    float2 rotatedOffset;
    float cosTheta = cos(input.rotation);
    float sinTheta = sin(input.rotation);
    rotatedOffset.x = cosTheta * offset.x - sinTheta * offset.y;
    rotatedOffset.y = sinTheta * offset.x + cosTheta * offset.y;
    
    float3 finalPosition = input.position + right * rotatedOffset.x * input.size.x + up * rotatedOffset.y * input.size.y;
    //float3 finalPosition = input.position + right * offset.x * input.size.x + up * offset.y * input.size.y;
    
    output.viewPosition = float4(finalPosition, 1.0);
    output.position = mul(output.viewPosition, Proj); 

    output.color = input.color;
    
    output.viewMat = View;
    
    output.particleCenter = float4(input.position, 1.0);

    // nearPlane < farPlane although currently using the reversed depth
    output.nearPlane = Proj._43 / (Proj._33 + 1.0);
    output.farPlane = -Proj._43 / Proj._33;

    
    // remaping texture coordinate to tile texture coordinate
    // ------------------------------------------------------
    float tileIndexFraction = (1 - input.lifetime) * totalTiles;
    int tileIndexRounded = (int) tileIndexFraction;
    int nextFrame = tileIndexRounded + 1;
    output.frameFraction = tileIndexFraction - tileIndexRounded; // This is the normalized time between the current and next frames

    //get the X (U) and Y (V) coordinates of tile
    int tileX = tileIndexRounded % tilesWidth;
    int tileY = tileIndexRounded / tilesWidth;
    
    // Normalizing the texcoord from [0, tilesWidth-1] and [0, tilesHeight-1] to [0, 1]
    float originTileU = tileX * tileUVWidth;
    float originTileV = tileY * tileUVHeight;

    // Remap the input UVs into the tile's UV space
    output.uv.x = originTileU + input.uv.x * tileUVWidth;
    output.uv.y = originTileV + input.uv.y * tileUVHeight;

    // Compute the UVs for the next frame
    int nextTileX = nextFrame % tilesWidth;
    int nextTileY = nextFrame / tilesWidth;
    float nextOriginTileU = nextTileX * tileUVWidth;
    float nextOriginTileV = nextTileY * tileUVHeight;
    output.uv2.x = nextOriginTileU + input.uv.x * tileUVWidth;
    output.uv2.y = nextOriginTileV + input.uv.y * tileUVHeight;

    // ------------------------------------------------------
    

    return output;
}
