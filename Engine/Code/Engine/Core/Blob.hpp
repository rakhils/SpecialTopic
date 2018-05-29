#pragma once

/*\class  : Blob
*		   
* \group  : <GroupName>
*		   
* \brief  :
*		   
* \TODO:  :
*		   
* \note   :
*		   
* \author : Rakhil Soman
*		   
* \version: 1.0
*		   
* \date   : 5/7/2018 3:06:16 PM
*
* \contact: srsrakhil@gmail.com
*/
 
class Blob
{
public:
	Blob();
	~Blob();

	// copy semantics
	Blob(Blob const &copy);
	Blob& operator=(Blob const &copy) = delete;

	// move semantics
	Blob(Blob &&obj_to_move);
	Blob& operator=(Blob &&move);

	// updates the blob to be a copy of the provided
	// memory.
	bool set(size_t const byte_count, void const *src);

	// update a section of the buffer located at offset
	// return false if this would not fit
	bool update(size_t const offset, size_t const byte_count, void const *src);

	// resizes the buffer, maintaining stored memory
	// if made smaller, should truncate.  If made larger
	// you can leave the newly allocated space as garbage.
	bool resize(size_t const new_size);

	inline void const* get_buffer() const { return m_buffer; }
	inline size_t get_size() const { return m_buffer_size; }


private:
	void *m_buffer;
	size_t m_buffer_size;
};
