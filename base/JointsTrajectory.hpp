#ifndef BASE_JOINTSTRAJECTORY_HPP
#define BASE_JOINTSTRAJECTORY_HPP

#include <vector>
#include <base/JointState.hpp>
#include <base/samples/Joints.hpp>
#include <base/NamedVector.hpp>
#include <base/Time.hpp>

namespace base {

typedef std::vector<JointState> JointTrajectory;

/** 
 * @brief Holds a time-series of JointStates for multiple joints
 *
 * This structure holds a time-series in the form of std::vector of JointState,
 * for multiple optionally named joints. 
 *
 * The time series can have optional time information attached to each sample
 * in the series through the times member.
 *
 * For the data-type to be valid, the length of each time series needs to be
 * the same for all joints. The times vector needs to be either empty or also
 * of that size
 *
 * How to access the state of a joint at a given sample:
 *      elements[joint_index][sample_number]
 * The first index corresponds to the indices in the name vector. Second index to the time step.
 *
 */
struct JointsTrajectory 
    : public NamedVector<JointTrajectory>
{
    /** Exception thrown when trying to access an element by
	 *  at an non-existing time step
	 */
	struct InvalidTimeStep : public std::runtime_error
	{
	    size_t time_step;
	    InvalidTimeStep(size_t const& time_step)
		: std::runtime_error("trying to access time_step which is out of range."),
		  time_step(time_step) {}

	    ~InvalidTimeStep() throw() {}
	};
	
    /**
     * @brief optional array of time values corresponding to the samples of the JointState
     *
     * This vector needs to be either empty or the same size as each of the
     * std::vector<JointState> in the elements vector.
     */
    std::vector<base::Time> times;

    /** @return true if the structure is valid
     */
    bool isValid() const
    {
	size_t samples = getTimeSteps();

	for(size_t i=0; i<elements.size(); ++i)
	{
	    if( elements[i].size() != samples )
		return false;
	}

	if( !times.empty() && times.size() != samples )
	    return false;

	return true;
    }
    
    void resize(int num_joints, int num_samples){
        this->resize(num_joints);
        for(size_t i=0; i<elements.size(); i++){
            elements[i].resize(num_samples);
        }
    }
    
    void resize(int num_joints){
        elements.resize(num_joints);
        names.resize(num_joints);
    }
    
    /**
     * @brief Extracts the base::samples::Joints structure at a given time step
     *
     * @param joints extracted base::samples::Joints will be stored here
     *
     * @throws InvalidTimeStep if the given time_step does not exist in the
     *         trajectory.
     */
    void getJointsAtTimeStep(size_t time_step, base::samples::Joints& joints){
        if(time_step > getTimeSteps())
            throw(InvalidTimeStep(time_step));
            
        joints.resize(getNumberOfJoints());
        joints.names = names;
        for(size_t i=0; i<getNumberOfJoints(); i++){
            joints.elements[i] = elements[i][time_step];
        }
    }

    /**
     * @return true if the JointState series has timing information
     */
    bool isTimed() const
    {
	return !times.empty();
    }

    /**
     * @return the number of time steps in the trajectory
     */
    size_t getTimeSteps() const
    {
	size_t steps = 0;
	if( !elements.empty() )
	    steps = elements[0].size();
	return steps;
    }
    
    /**
     * @return the number of joints in the trajectory
     */
    size_t getNumberOfJoints() const
    {
	return elements.size();
    }
   
    /** 
     * @return the total duration of the time series if time information is available
     */
    base::Time getDuration() const
    {
	base::Time summed;
	for(size_t i=0; i<times.size(); i++)
	{
	    summed = summed+times[i];
	}
	return summed;
    }
}; 

}

#endif

