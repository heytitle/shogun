#include <shogun/base/SGObject.h>
#include <shogun/base/range.h>
#include <shogun/base/some.h>
#include <shogun/lib/SGMatrix.h>
#include <shogun/lib/SGSparseMatrix.h>
#include <shogun/lib/SGSparseVector.h>
#include <shogun/lib/SGString.h>
#include <shogun/lib/SGVector.h>

namespace shogun
{
	/** @brief Mock class to test clone and equals for CSGObject.
	 * Serves as a parameter of type CSGObject for @see CCloneEqualsMock.
	 */
	template <class T>
	class CCloneEqualsMockParameter : public CSGObject
	{
	public:
		CCloneEqualsMockParameter()
		{
			m_was_cloned = false;
			m_some_value = 1;

			watch_param("some_value", &m_some_value);
		}
		const char* get_name() const
		{
			return "CloneEqualsMockParameter";
		}

		virtual CSGObject* clone()
		{
			auto* clone = new CCloneEqualsMockParameter<T>();
			clone->m_was_cloned = true;
			SG_REF(clone);
			return clone;
		}

		bool m_was_cloned;
		T m_some_value;
	};

	/** @brief Mock class to test clone and equals for CSGObject.
	 * Has members that cover (hopefully) all possibilities of parameters.
	 */
	template <class T>
	class CCloneEqualsMock : public CSGObject
	{
	public:
		CCloneEqualsMock()
		{
			m_basic = 1;
			watch_param("basic", &m_basic);

			m_object = new CCloneEqualsMockParameter<T>();
			watch_param("object", &m_object);

			m_sg_vector = SGVector<T>(2);
			m_sg_vector.set_const(m_basic);
			watch_param("sg_vector", &m_sg_vector);

			m_sg_matrix = SGMatrix<T>(3, 4);
			m_sg_matrix.set_const(m_basic);
			watch_param("sg_matrix", &m_sg_matrix);

			m_sg_sparse_vector = SGSparseVector<T>(4);
			for (auto i : range(m_sg_sparse_vector.num_feat_entries))
			{
				SGSparseVectorEntry<T> entry;
				entry.feat_index = i * 2;
				entry.entry = 2;
				m_sg_sparse_vector.features[i] = entry;
			}
			watch_param("sg_sparse_vector", &m_sg_sparse_vector);

			m_sg_sparse_matrix =
			    SGSparseMatrix<T>(m_sg_sparse_vector.num_feat_entries, 6);
			for (auto i : range(m_sg_sparse_matrix.num_vectors))
			{
				SGSparseVector<T> vec(m_sg_sparse_matrix.num_features);
				for (auto j : range(vec.num_feat_entries))
				{
					SGSparseVectorEntry<T> entry;
					entry.feat_index = i * 2;
					entry.entry = 3;
					vec.features[j] = entry;
				}
				m_sg_sparse_matrix.sparse_matrix[i] = vec;
			}
			watch_param("sg_sparse_matrix", &m_sg_sparse_matrix);

			m_vector_basic_len = 5;
			m_vector_basic = new T[m_vector_basic_len];
			for (auto i : range(m_vector_basic_len))
				m_vector_basic[i] = m_basic;
			watch_param("vector_basic", &m_vector_basic, &m_vector_basic_len);

			m_vector_sg_string_len = 7;
			m_vector_sg_string = new SGString<T>[m_vector_sg_string_len];
			for (auto i : range(m_vector_sg_string_len))
			{
				m_vector_sg_string[i] = SGString<T>(i + 1, true);
				for (auto j : range(m_vector_sg_string[i].slen))
					m_vector_sg_string[i].string[j] = 1;
			}
			watch_param(
			    "vector_sg_string", &m_vector_sg_string,
			    &m_vector_sg_string_len);

			m_vector_object_len = 6;
			m_vector_object =
			    new CCloneEqualsMockParameter<T>*[m_vector_object_len];
			for (auto i : range(m_vector_object_len))
				m_vector_object[i] = new CCloneEqualsMockParameter<T>();
			watch_param(
			    "vector_object", &m_vector_object, &m_vector_object_len);
		}

		~CCloneEqualsMock()
		{
			delete m_object;
			delete m_vector_basic;
			for (auto i : range(m_vector_object_len))
				delete m_vector_object[i];
			delete m_vector_object;

			for (auto i : range(m_vector_sg_string_len))
				m_vector_sg_string[i].free_string();
			delete m_vector_sg_string;
		}

		const char* get_name() const
		{
			return "CloneEqualsMock";
		}

		T m_basic;
		CCloneEqualsMockParameter<T>* m_object;

		SGVector<T> m_sg_vector;
		SGMatrix<T> m_sg_matrix;

		SGSparseVector<T> m_sg_sparse_vector;
		SGSparseMatrix<T> m_sg_sparse_matrix;

		T* m_vector_basic;
		index_t m_vector_basic_len;

		SGString<T>* m_vector_sg_string;
		index_t m_vector_sg_string_len;

		CCloneEqualsMockParameter<T>** m_vector_object;
		index_t m_vector_object_len;
	};

	/** @brief Used to test the tags-parameter framework
	 * Allows testing of registering new member and avoiding
	 * non-registered member variables using tags framework.
	 */
	class CMockObject : public CSGObject
	{
	public:
		CMockObject() : CSGObject()
		{
			init_params();
		}

		virtual ~CMockObject()
		{
			SG_UNREF(m_object);
		}

		const char* get_name() const
		{
			return "MockObject";
		}

		void set_watched(int32_t value)
		{
			m_watched = value;
		}

		int32_t get_watched() const
		{
			return m_watched;
		}

		CSGObject* get_object() const
		{
			return m_object;
		}

	protected:
		void init_params()
		{
			float64_t decimal = 0.0;
			register_param("vector", SGVector<float64_t>());
			register_param("int", m_integer);
			register_param("float", decimal);

			watch_param(
			    "watched_int", &m_watched,
			    AnyParameterProperties(
			        MS_NOT_AVAILABLE, GRADIENT_NOT_AVAILABLE));

			watch_param(
			    "watched_object", (CSGObject**)&m_object,
			    AnyParameterProperties(
			        MS_NOT_AVAILABLE, GRADIENT_NOT_AVAILABLE));
		}

	private:
		int32_t m_integer = 0;
		int32_t m_watched = 0;

		CSGObject* m_object = 0;
	};
}
