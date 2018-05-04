/*
   Copyright (c) 2009-2016, Jack Poulson
   All rights reserved.

   Copyright (c) 2012 Jack Poulson, Lexing Ying, and
   The University of Texas at Austin.
   All rights reserved.

   Copyright (c) 2013 Jack Poulson, Lexing Ying, and Stanford University.
   All rights reserved.

   Copyright (c) 2014 Jack Poulson and The Georgia Institute of Technology.
   All rights reserved.

   This file is part of Elemental and is under the BSD 2-Clause License,
   which can be found in the LICENSE file in the root directory, or at
   http://opensource.org/licenses/BSD-2-Clause
*/
#ifndef EL_FACTOR_LDL_SPARSE_NUMERIC_HPP
#define EL_FACTOR_LDL_SPARSE_NUMERIC_HPP

#define EL_SUITESPARSE_NO_SCALAR_FUNCS
#include <ElSuiteSparse/ldl.hpp>

namespace El {

enum LDLFrontType
{
  SYMM_1D,                SYMM_2D,
  LDL_1D,                 LDL_2D,
  LDL_SELINV_1D,          LDL_SELINV_2D,
  LDL_INTRAPIV_1D,        LDL_INTRAPIV_2D,
  LDL_INTRAPIV_SELINV_1D, LDL_INTRAPIV_SELINV_2D,
  BLOCK_LDL_1D,           BLOCK_LDL_2D,
  BLOCK_LDL_INTRAPIV_1D,  BLOCK_LDL_INTRAPIV_2D
};

bool Unfactored( LDLFrontType type );
bool FrontIs1D( LDLFrontType type );
bool BlockFactorization( LDLFrontType type );
bool SelInvFactorization( LDLFrontType type );
bool PivotedFactorization( LDLFrontType type );
LDLFrontType ConvertTo2D( LDLFrontType type );
LDLFrontType ConvertTo1D( LDLFrontType type );
LDLFrontType AppendSelInv( LDLFrontType type );
LDLFrontType RemoveSelInv( LDLFrontType type );
LDLFrontType InitialFactorType( LDLFrontType type );

namespace ldl {

template<typename T>
struct DistMatrixNode;
template<typename T>
struct DistMultiVecNode;

template<typename T>
struct MatrixNode
{
    Matrix<T> matrix;
    Matrix<T> work;

    MatrixNode<T>* parent;
    vector<MatrixNode<T>*> children;
    DistMatrixNode<T>* duplicateMat;
    DistMultiVecNode<T>* duplicateMV;

    MatrixNode( MatrixNode<T>* parentNode=nullptr );
    MatrixNode( DistMatrixNode<T>* dupNode );
    MatrixNode( DistMultiVecNode<T>* dupNode );

    MatrixNode
    ( const vector<Int>& invMap,
      const NodeInfo& info,
      const Matrix<T>& X );

    ~MatrixNode();

    const MatrixNode<T>& operator=( const MatrixNode<T>& X );

    void Pull
    ( const vector<Int>& invMap,
      const NodeInfo& info,
      const Matrix<T>& X );
    void Push
    ( const vector<Int>& invMap,
      const NodeInfo& info,
            Matrix<T>& X ) const;

    Int Height() const;
};

struct MultiVecCommMeta
{
    Int localOff, localSize;
    vector<int> numChildSendInds;
    vector<vector<Int>> childRecvInds;

    void Empty()
    {
        SwapClear( numChildSendInds );
        SwapClear( childRecvInds );
    }
};

struct DistMultiVecNodeMeta
{
    vector<Int> sendInds;
    vector<Int> recvInds;
    vector<int> mappedOwners;
    vector<int> sendSizes;
    vector<int> sendOffs;
    vector<int> recvSizes;
    vector<int> recvOffs;

    template<typename T>
    void Initialize
    ( const DistMultiVecNode<T>& XNode,
      const DistNodeInfo& info,
      const DistMap& invMap,
      const DistMultiVec<T>& X );
};

// For handling a set of vectors distributed in a [VC,* ] manner over each node
// of the elimination tree
template<typename T>
struct DistMultiVecNode
{
    DistMatrix<T,VC,STAR> matrix;
    DistMatrix<T,VC,STAR> work;

    DistMultiVecNode<T>* parent;
    DistMultiVecNode<T>* child;
    MatrixNode<T>* duplicate;

    mutable MultiVecCommMeta commMeta;

    DistMultiVecNode( DistMultiVecNode<T>* parentNode=nullptr );

    DistMultiVecNode
    ( const DistMap& invMap,
      const DistNodeInfo& info,
      const DistMultiVec<T>& X );

    DistMultiVecNode( const DistMatrixNode<T>& X );

    ~DistMultiVecNode();

    const DistMultiVecNode<T>& operator=( const DistMatrixNode<T>& X );

    void Pull
    ( const DistMap& invMap,
      const DistNodeInfo& info,
      const DistMultiVec<T>& X );
    void Pull
    ( const DistMap& invMap,
      const DistNodeInfo& info,
      const DistMultiVec<T>& X,
            DistMultiVecNodeMeta& meta );
    void Push
    ( const DistMap& invMap,
      const DistNodeInfo& info,
            DistMultiVec<T>& X ) const;
    void Push
    ( const DistMap& invMap,
      const DistNodeInfo& info,
            DistMultiVec<T>& X,
            DistMultiVecNodeMeta& meta ) const;

    Int LocalHeight() const;
    void ComputeCommMeta( const DistNodeInfo& info ) const;
};

struct MatrixCommMeta
{
    vector<int> numChildSendInds;
    vector<vector<Int>> childRecvInds;

    void Empty()
    {
        SwapClear( numChildSendInds );
        SwapClear( childRecvInds );
    }
};

// For handling a matrix distributed in a [MC,MR] manner over each node
// of the elimination tree
template<typename T>
struct DistMatrixNode
{
    DistMatrix<T> matrix;
    DistMatrix<T> work;

    DistMatrixNode<T>* parent;
    DistMatrixNode<T>* child;
    MatrixNode<T>* duplicate;

    mutable MatrixCommMeta commMeta;

    DistMatrixNode( DistMatrixNode<T>* parentNode=nullptr );

    DistMatrixNode
    ( const DistMap& invMap, const DistNodeInfo& info,
      const DistMultiVec<T>& X );

    DistMatrixNode( const DistMultiVecNode<T>& X );

    ~DistMatrixNode();

    const DistMatrixNode<T>& operator=( const DistMultiVecNode<T>& X );

    void Pull
    ( const DistMap& invMap, const DistNodeInfo& info,
      const DistMultiVec<T>& X );
    void Push
    ( const DistMap& invMap, const DistNodeInfo& info,
            DistMultiVec<T>& X ) const;

    void ComputeCommMeta( const DistNodeInfo& info ) const;
};

// Only keep track of the left and bottom-right piece of the fronts
// (with the bottom-right piece stored in workspace) since only the left side
// needs to be kept after the factorization is complete.

template<typename Field>
struct DistFront;

template<typename Field>
struct Front
{
    bool isHermitian;
    bool sparseLeaf;
    LDLFrontType type;

    Matrix<Field> LDense;
    SparseMatrix<Field> LSparse;

    Matrix<Field> diag;
    Matrix<Field> subdiag;
    Permutation p;

    Matrix<Field> workDense;
    SparseMatrix<Field> workSparse;

    Front<Field>* parent;
    vector<Front<Field>*> children;
    DistFront<Field>* duplicate;

    Front( Front<Field>* parentNode=nullptr );
    Front( DistFront<Field>* dupNode );
    Front
    ( const SparseMatrix<Field>& A,
      const vector<Int>& reordering,
      const NodeInfo& rootInfo,
      bool conjugate=true );

    ~Front();

    void Pull
    ( const SparseMatrix<Field>& A,
      const vector<Int>& reordering,
      const NodeInfo& rootInfo,
      bool conjugate=true );
    void PullUpdate
    ( const SparseMatrix<Field>& A,
      const vector<Int>& reordering,
      const NodeInfo& rootInfo );

    void Push
    (       SparseMatrix<Field>& A,
      const vector<Int>& reordering,
      const NodeInfo& rootInfo ) const;

    void Unpack( SparseMatrix<Field>& A, const NodeInfo& rootInfo ) const;

    const Front<Field>& operator=( const Front<Field>& front );

    Int Height() const;
    Int NumEntries() const;
    Int NumTopLeftEntries() const;
    Int NumBottomLeftEntries() const;
    double FactorGFlops() const;
    double SolveGFlops( Int numRHS=1 ) const;
};

struct FactorCommMeta
{
    vector<int> numChildSendInds;
    // This information does not necessarily have to be kept and can be
    // computed from the above information (albeit somewhat expensively).
    mutable vector<vector<Int>> childRecvInds;

    void EmptyChildRecvIndices() const
    { SwapClear(childRecvInds); }

    void Empty()
    {
        SwapClear( numChildSendInds );
        EmptyChildRecvIndices();
    }
};
void ComputeFactRecvInds( const DistNodeInfo& info );

template<typename Field>
struct DistFront
{
    bool isHermitian;
    LDLFrontType type;

    // Split each front into a left and right piece such that the right piece
    // is not needed after the factorization (and can be freed).

    // When this node is a duplicate of a sequential node, L1D or L2D will be
    // attached to the sequential L matrix of the duplicate

    DistMatrix<Field,VC,STAR> L1D;
    DistMatrix<Field> L2D;

    DistMatrix<Field,VC,STAR> diag;
    DistMatrix<Field,VC,STAR> subdiag;
    DistPermutation p;

    DistMatrix<Field> work;
    mutable FactorCommMeta commMeta;

    DistFront<Field>* parent;
    DistFront<Field>* child;
    Front<Field>* duplicate;

    DistFront( DistFront<Field>* parentNode=nullptr );

    DistFront
    ( const DistSparseMatrix<Field>& A,
      const DistMap& reordering,
      const DistSeparator& rootSep,
      const DistNodeInfo& info,
      bool conjugate=false );

    ~DistFront();

    void Pull
    ( const DistSparseMatrix<Field>& A,
      const DistMap& reordering,
      const DistSeparator& rootSep,
      const DistNodeInfo& info,
      bool conjugate=false );
    // Allow the reuse of mapped{Sources,Targets}, which are expensive to form
    void Pull
    ( const DistSparseMatrix<Field>& A,
      const DistMap& reordering,
      const DistSeparator& rootSep,
      const DistNodeInfo& info,
            vector<Int>& mappedSources,
            vector<Int>& mappedTargets,
            vector<Int>& colOffs,
      bool conjugate=false );

    void PullUpdate
    ( const DistSparseMatrix<Field>& A,
      const DistMap& reordering,
      const DistSeparator& rootSep,
      const DistNodeInfo& info );
    // Allow the reuse of mapped{Sources,Targets}, which are expensive to form
    void PullUpdate
    ( const DistSparseMatrix<Field>& A,
      const DistMap& reordering,
      const DistSeparator& rootSep,
      const DistNodeInfo& info,
            vector<Int>& mappedSources,
            vector<Int>& mappedTargets,
            vector<Int>& colOffs );

    // NOTE: This routine is not yet functioning
    void Push
    ( DistSparseMatrix<Field>& A, const DistMap& reordering,
      const DistSeparator& rootSep, const DistNodeInfo& rootInfo ) const;

    void Unpack
    ( DistSparseMatrix<Field>& A,
      const DistSeparator& rootSep, const DistNodeInfo& rootInfo ) const;

    const DistFront<Field>& operator=( const DistFront<Field>& front );

    Int NumLocalEntries() const;
    Int NumTopLeftLocalEntries() const;
    Int NumBottomLeftLocalEntries() const;
    double LocalFactorGFlops( bool selInv=false ) const;
    double LocalSolveGFlops( Int numRHS=1 ) const;

    void ComputeRecvInds( const DistNodeInfo& info ) const;
    void ComputeCommMeta
    ( const DistNodeInfo& info, bool computeRecvInds ) const;
};

template<typename Field>
void ChangeFrontType
( Front<Field>& front, LDLFrontType type, bool recurse=true );
template<typename Field>
void ChangeFrontType
( DistFront<Field>& front, LDLFrontType type, bool recurse=true );

template<typename Field>
void DiagonalScale
( const NodeInfo& info, const Front<Field>& front, MatrixNode<Field>& X );
template<typename Field>
void DiagonalScale
( const DistNodeInfo& info,
  const DistFront<Field>& L,
        DistMultiVecNode<Field>& X );
template<typename Field>
void DiagonalScale
( const DistNodeInfo& info,
  const DistFront<Field>& L,
        DistMatrixNode<Field>& X );

template<typename Field>
void DiagonalSolve
( const NodeInfo& info,
  const Front<Field>& front,
        MatrixNode<Field>& X );
template<typename Field>
void DiagonalSolve
( const DistNodeInfo& info,
  const DistFront<Field>& front,
        DistMultiVecNode<Field>& X );
template<typename Field>
void DiagonalSolve
( const DistNodeInfo& info,
  const DistFront<Field>& L,
        DistMatrixNode<Field>& X );

template<typename Field>
void LowerSolve
( Orientation orientation, const NodeInfo& info,
  const Front<Field>& L, MatrixNode<Field>& X );
template<typename Field>
void LowerSolve
( Orientation orientation, const DistNodeInfo& info,
  const DistFront<Field>& L, DistMultiVecNode<Field>& X );
template<typename Field>
void LowerSolve
( Orientation orientation, const DistNodeInfo& info,
  const DistFront<Field>& L, DistMatrixNode<Field>& X );

template<typename Field>
void LowerMultiply
( Orientation orientation, const NodeInfo& info,
  const Front<Field>& L, MatrixNode<Field>& X );
template<typename Field>
void LowerMultiply
( Orientation orientation, const DistNodeInfo& info,
  const DistFront<Field>& L, DistMultiVecNode<Field>& X );
template<typename Field>
void LowerMultiply
( Orientation orientation, const DistNodeInfo& info,
  const DistFront<Field>& L, DistMatrixNode<Field>& X );

} // namespace ldl
} // namespace El

#endif // ifndef EL_FACTOR_LDL_SPARSE_NUMERIC_HPP