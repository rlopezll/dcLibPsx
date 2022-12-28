
#include "dcMesh.h"
#include "dcMath.h"
#include "dcRender.h"

void dcMeshP_Record(SDC_Mesh* mesh, SDC_DrawParams* drawParams, u_long* ot, long otSize, SDC_StackAllocator* renderMemory);
void dcMeshPC_Record(SDC_Mesh* mesh, SDC_DrawParams* drawParams, u_long* ot, long otSize, SDC_StackAllocator* renderMemory);
void dcMeshPCN_Record(SDC_Mesh* mesh, SDC_DrawParams* drawParams, u_long* ot, long otSize, SDC_StackAllocator* renderMemory);
void dcMeshPT_Record(SDC_Mesh* mesh, SDC_DrawParams* drawParams, u_long* ot, long otSize, SDC_StackAllocator* renderMemory);
void dcMeshPTN_Record(SDC_Mesh* mesh, SDC_DrawParams* drawParams, u_long* ot, long otSize, SDC_StackAllocator* renderMemory);
void dcMeshPCT_Record(SDC_Mesh* mesh, SDC_DrawParams* drawParams, u_long* ot, long otSize, SDC_StackAllocator* renderMemory);
void dcMeshPCTN_Record(SDC_Mesh* mesh, SDC_DrawParams* drawParams, u_long* ot, long otSize, SDC_StackAllocator* renderMemory);

typedef void(*FPtrDC_RecordMesh)(SDC_Mesh*, SDC_DrawParams*, u_long*, long, SDC_StackAllocator*);

FPtrDC_RecordMesh recordMeshFuncPtrs[DCVFMT_NUM_FORMATS] = 
{
    &dcMeshP_Record,
    &dcMeshPC_Record,
    &dcMeshPCN_Record,
    &dcMeshPT_Record,
    &dcMeshPTN_Record,
    &dcMeshPCT_Record,
    &dcMeshPCTN_Record
};

void dcMesh_Record(SDC_Mesh* mesh, SDC_DrawParams* drawParams, u_long* ot, long otSize, SDC_StackAllocator* renderMemory)
{
    FPtrDC_RecordMesh funcPtr = recordMeshFuncPtrs[mesh->vtxFormat];
    (*funcPtr)(mesh, drawParams, ot, otSize, renderMemory);
};

void dcMeshP_Record(SDC_Mesh* mesh, SDC_DrawParams* drawParams, u_long* ot, long otSize, SDC_StackAllocator* renderMemory)
{

}

void dcMeshPC_Record(SDC_Mesh* mesh, SDC_DrawParams* drawParams, u_long* ot, long otSize, SDC_StackAllocator* renderMemory)
{

}

void dcMeshPCN_Record(SDC_Mesh* mesh, SDC_DrawParams* drawParams, u_long* ot, long otSize, SDC_StackAllocator* renderMemory)
{

}

void dcMeshPT_Record(SDC_Mesh* mesh, SDC_DrawParams* drawParams, u_long* ot, long otSize, SDC_StackAllocator* renderMemory)
{

}

void dcMeshPTN_Record(SDC_Mesh* mesh, SDC_DrawParams* drawParams, u_long* ot, long otSize, SDC_StackAllocator* renderMemory)
{

}

void dcMeshPCT_Record(SDC_Mesh* mesh, SDC_DrawParams* drawParams, u_long* ot, long otSize, SDC_StackAllocator* renderMemory)
{

}

void dcMeshPCTN_Record(SDC_Mesh* mesh, SDC_DrawParams* drawParams, u_long* ot, long otSize, SDC_StackAllocator* renderMemory)
{

}
